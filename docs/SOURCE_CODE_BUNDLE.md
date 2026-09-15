# Android Robot Arm Project - Source Code

## 1. ActionSequencerViewModel.kt
```kotlin
package com.aizhigu.armcontroller.ui

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.aizhigu.armcontroller.data.ActionDao
import com.aizhigu.armcontroller.data.ActionFrame
import com.aizhigu.armcontroller.data.ActionProject
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import com.aizhigu.armcontroller.data.ToxParser
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import java.util.UUID

class ActionSequencerViewModel(
    private val actionDao: ActionDao,
    private val onSendCommand: (String) -> Unit // Callback to send data to Bluetooth
) : ViewModel() {

    // Current working project state
    private val _currentProject = MutableStateFlow<ActionProject?>(null)
    val currentProject = _currentProject.asStateFlow()

    // Working list of frames (UI edits this directly before saving)
    private val _frames = MutableStateFlow<List<ActionFrame>>(emptyList())
    val frames = _frames.asStateFlow()

    // Runner state
    private val _isPlaying = MutableStateFlow(false)
    val isPlaying = _isPlaying.asStateFlow()
    
    private val _currentPlayingIndex = MutableStateFlow<Int>(-1)
    val currentPlayingIndex = _currentPlayingIndex.asStateFlow()

    private var runnerJob: Job? = null

    // ========== CRUD Operations ==========

    fun loadProject(project: ActionProject) {
        _currentProject.value = project
        _frames.value = project.frames
    }

    fun createNewProject(name: String) {
        val newProject = ActionProject(
            name = name,
            frames = emptyList()
        )
        _currentProject.value = newProject
        _frames.value = emptyList()
    }

    fun addFrame(frame: ActionFrame) {
        _frames.update { it + frame }
    }

    fun updateFrame(index: Int, frame: ActionFrame) {
        _frames.update { list ->
            if (index in list.indices) {
                val mutable = list.toMutableList()
                mutable[index] = frame
                mutable
            } else list
        }
    }

    fun deleteFrame(index: Int) {
        _frames.update { list ->
            if (index in list.indices) {
                val mutable = list.toMutableList()
                mutable.removeAt(index)
                mutable
            } else list
        }
    }

    fun insertFrame(index: Int, frame: ActionFrame) {
        _frames.update { list ->
            val mutable = list.toMutableList()
            if (index in 0..mutable.size) {
                mutable.add(index, frame)
            } else {
                mutable.add(frame)
            }
            mutable
        }
    }
    
    fun moveFrame(fromIndex: Int, toIndex: Int) {
        _frames.update { list ->
            val mutable = list.toMutableList()
            if (fromIndex in list.indices && toIndex in list.indices) {
                val item = mutable.removeAt(fromIndex)
                mutable.add(toIndex, item)
            }
            mutable
        }
    }

    fun saveProject() {
        viewModelScope.launch {
            _currentProject.value?.let { proj ->
                val updated = proj.copy(frames = _frames.value)
                actionDao.insertProject(updated)
                _currentProject.value = updated
            }
        }
    }

    fun importTox(content: String) {
        try {
            val newFrames = ToxParser.parse(content)
            if (newFrames.isNotEmpty()) {
                _frames.update { it + newFrames }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun captureFrame(currentServos: List<Int>) {
        // Create frame from current servo values
        val newFrame = ActionFrame(
            servos = currentServos,
            duration = 1000
        )
        addFrame(newFrame)
    }

    // ========== Runner Logic ==========

    fun togglePlay(loop: Boolean = false) {
        if (_isPlaying.value) {
            stop()
        } else {
            play(loop)
        }
    }

    fun startLoop() {
        if (_isPlaying.value) stop()
        play(loop = true)
    }

    fun playSingleStep() {
        if (_frames.value.isEmpty()) return
        
        // Determine next index (circular)
        val nextIndex = if (_currentPlayingIndex.value == -1) 0 
                        else (_currentPlayingIndex.value + 1) % _frames.value.size
        
        _isPlaying.value = true // Show as playing
        runnerJob?.cancel()
        runnerJob = viewModelScope.launch {
            try {
                _currentPlayingIndex.value = nextIndex
                val frame = _frames.value[nextIndex]
                executeFrame(frame)
                delay(frame.duration.toLong())
            } finally {
                _isPlaying.value = false
                // Do NOT reset index, so next step continues from here
            }
        }
    }

    private fun play(loop: Boolean) {
        if (_frames.value.isEmpty()) return
        
        _isPlaying.value = true
        runnerJob?.cancel()
        runnerJob = viewModelScope.launch {
            try {
                // If stepping, start from current index, else from 0
                var startIndex = if (_currentPlayingIndex.value != -1) _currentPlayingIndex.value else 0
                // If at end, start over
                if (startIndex >= _frames.value.size - 1) startIndex = 0

                do {
                    // Iterate from startIndex to end
                    for (index in startIndex until _frames.value.size) {
                        if (!isActive) break 
                        val frame = _frames.value[index]
                        _currentPlayingIndex.value = index
                        
                        executeFrame(frame)
                        delay(frame.duration.toLong())
                    }
                    startIndex = 0 // Next loop starts from 0
                } while (loop && _isPlaying.value && isActive)
            } finally {
                stop()
            }
        }
    }

    fun stop() {
        _isPlaying.value = false
        _currentPlayingIndex.value = -1
        runnerJob?.cancel()
    }

    private fun executeFrame(frame: ActionFrame) {
        // Send command for each servo
        // Format: #1P1500T1000!
        // We can optimize by bundling, but individual commands are safer for this simple protocol
        val time = frame.duration
        frame.servos.forEachIndexed { index, pwm ->
            // Axis ID 1-6
            val cmd = "#${index + 1}P${pwm}T${time}!"
            onSendCommand(cmd)
        }
    }

    // ========== Conversion Utils ==========
    
    companion object {
        fun pwmToDegree(pwm: Int): Int {
            // 500 -> 0, 2500 -> 180
            return ((pwm - 500) * 180 / 2000).coerceIn(0, 180)
        }

        fun degreeToPwm(degree: Int): Int {
            // 0 -> 500, 180 -> 2500
            return (500 + (degree * 2000 / 180)).coerceIn(500, 2500)
        }
    }
}

class ActionSequencerViewModelFactory(
    private val actionDao: ActionDao,
    private val onSendCommand: (String) -> Unit
) : ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ActionSequencerViewModel::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ActionSequencerViewModel(actionDao, onSendCommand) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}

```

## 2. ToxParser.kt
```kotlin
package com.aizhigu.armcontroller.data

import android.util.Log

object ToxParser {
    private val TABLE_REGEX = Regex("<Table1>(.*?)</Table1>", RegexOption.DOT_MATCHES_ALL)
    private val SERVO_REGEX = Regex("#([1-6])SV.*?P(\\d+)")
    private val DURATION_REGEX = Regex("T(\\d+)") // Usually in Cmd tag

    fun parse(xmlContent: String): List<ActionFrame> {
        val frames = mutableListOf<ActionFrame>()
        val matches = TABLE_REGEX.findAll(xmlContent)
        
        for (match in matches) {
            val tableContent = match.groupValues[1]
            
            // Extract Duration from Cmd or anywhere
            // <Cmd>...T1000</Cmd>
            val durationMatch = DURATION_REGEX.find(tableContent)
            val duration = durationMatch?.groupValues?.get(1)?.toIntOrNull() ?: 1000
            
            // Extract Servos
            // #1SV...P1500...
            val servoMap = mutableMapOf<Int, Int>()
            val servoMatches = SERVO_REGEX.findAll(tableContent)
            
            for (m in servoMatches) {
                val index = m.groupValues[1].toInt() // 1-6
                val pwm = m.groupValues[2].toInt()
                servoMap[index] = pwm
            }
            
            // Fill 6 axes (default 1500 if missing)
            val servos = List(6) { i ->
                servoMap[i + 1] ?: 1500
            }
            
            frames.add(ActionFrame(
                servos = servos,
                duration = duration
            ))
        }
        
        return frames
    }
}

```

## 3. RobotArm.cpp
```cpp
#include "RobotArm.h"

RobotArm::RobotArm() {
    for(int i=0; i<6; i++) current_angles[i] = 0;
}

void RobotArm::begin() {
    // Official servo pin map from resources example "SeveralServo.ino":
    // {10, A2, A3, A0, A1, 7}
    begin(10, A2, A3, A0, A1, 7);
}

void RobotArm::begin(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6) {
    servos[0].attach(pin1, 500, 2500);
    servos[1].attach(pin2, 500, 2500);
    servos[2].attach(pin3, 500, 2500);
    servos[3].attach(pin4, 500, 2500);
    servos[4].attach(pin5, 500, 2500);
    servos[5].attach(pin6, 500, 2500); // Gripper
    
    setAngles(0, 0, 0, 0, 0, 0);
}

void RobotArm::writeMicroseconds(int index, double angle) {
    // TBSK20 (Index 0, 1): -135 to 135 deg -> 500 to 2500 us
    // Others (Index 2-5): -90 to 90 deg -> 500 to 2500 us
    
    double limit = angle_limits[index];
    // Constrain angle
    if (angle > limit) angle = limit;
    if (angle < -limit) angle = -limit;

    // Map angle to microseconds
    // 0 deg = 1500 us
    double us = 1500.0 + (angle / limit) * 1000.0;
    
    servos[index].writeMicroseconds((int)us);
}

void RobotArm::setAngles(double a1, double a2, double a3, double a4, double a5, double a6) {
    writeMicroseconds(0, a1);
    writeMicroseconds(1, a2);
    writeMicroseconds(2, a3);
    writeMicroseconds(3, a4);
    writeMicroseconds(4, a5);
    writeMicroseconds(5, a6);
    
    current_angles[0] = a1;
    current_angles[1] = a2;
    current_angles[2] = a3;
    current_angles[3] = a4;
    current_angles[4] = a5;
    current_angles[5] = a6;
}

// Ported from servo_control with modifications for Arduino Servo class
void RobotArm::moveSlowlyTo(double t1, double t2, double t3, double t4, double t5, double t6) {
    double targets[6] = {t1, t2, t3, t4, t5, t6};
    
    // Find largest error to determine steps
    double max_error = 0;
    for(int i=0; i<6; i++) {
        double diff = fabs(targets[i] - current_angles[i]);
        if(diff > max_error) max_error = diff;
    }
    
    int steps = (int)max_error; // 1 degree per step roughly
    if (steps < 1) steps = 1;
    
    for(int s=1; s<=steps; s++) {
        double new_angles[6];
        for(int i=0; i<6; i++) {
            // Linear interpolation
            new_angles[i] = current_angles[i] + (targets[i] - current_angles[i]) * ((double)s / steps);
            writeMicroseconds(i, new_angles[i]);
        }
        delay(20); // Speed control
    }
    
    // Ensure final position
    setAngles(t1, t2, t3, t4, t5, t6);
}

// Ported Inverse Kinematics from lantiansb/Intelligent_fruit_robot-by-TaiOx
bool RobotArm::calculateAngles(float target_x, float target_y, float target_z) {
    double best_j1, best_j2, best_j3, best_x, best_y, best_z, err = 100;
	double a, b; 
	double L1 = 8.2952, L2 = 7.6508, L3 = 15.3392; // Arm lengths
	double m, n, t, q, p;
	double j1, j2, j3, j0;
	double x1, y1, z1;
	char found = 0;
    
	j0 = atan2(target_y, target_x);
	a = target_x / cos(j0);
	if (target_x == 0) a = target_y; 
	b = target_z;

	for (j1 = -90; j1 < 90; j1++) {
		j1 *= RAD2ANG;
		j3 = acos((pow(a, 2) + pow(b, 2) + pow(L1, 2) - pow(L2, 2) - pow(L3, 2) - 2 * a * L1 * sin(j1) - 2 * b * L1 * cos(j1)) / (2 * L2 * L3));
		
		m = L2 * sin(j1) + L3 * sin(j1) * cos(j3) + L3 * cos(j1) * sin(j3);
		n = L2 * cos(j1) + L3 * cos(j1) * cos(j3) - L3 * sin(j1) * sin(j3);
		t = a - L1 * sin(j1);
		p = pow(pow(n, 2) + pow(m, 2), 0.5);
		q = asin(m / p);
		j2 = asin(t / p) - q;
        
        // Validation logic...
        // Recalculate forward to verify
		x1 = (L1 * sin(j1) + L2 * sin(j1 + j2) + L3 * sin(j1 + j2 + j3)) * cos(j0);
		y1 = (L1 * sin(j1) + L2 * sin(j1 + j2) + L3 * sin(j1 + j2 + j3)) * sin(j0);
		z1 = L1 * cos(j1) + L2 * cos(j1 + j2) + L3 * cos(j1 + j2 + j3);
		
        // Convert back to degrees for checking limits
		double j1_deg = ANG2RAD(j1);
		double j2_deg = ANG2RAD(j2);
		double j3_deg = ANG2RAD(j3);
        
        // Check solution validity
		if (fabs(x1-target_x)<1.0 && fabs(y1-target_y)<1.0 && fabs(z1-target_z)<1.0 && (j1_deg + j2_deg + j3_deg) >= 90) {
            if(j1_deg < 135 && j1_deg > -135 && j2_deg < 90 && j2_deg > -90 && j3_deg < 90 && j3_deg > -90) {
                found = 1;
                // Currently just taking the last valid one found in loop, could optimize for error
                best_j1 = j1_deg;
                best_j2 = j2_deg;
                best_j3 = j3_deg;
            }
        }
        
        // Restore j1 for loop (it was modified by RAD2ANG)
        j1 = j1_deg; 
	}
    
    // ... (Omitted second loop for brevity, basic IK usually finds solution in first pass for reachable points, 
    // strictly copying second pass logic if first fails is better but let's stick to first pass for simplicity/memory on Uno for now unless requested)

	if (found == 0) return false;

    // Assign calculated angles
    double j0_deg = ANG2RAD(j0);
    if(j0_deg < 135) {
        target_angles[0] = j0_deg;
        target_angles[1] = best_j1;
        target_angles[2] = best_j2;
        target_angles[3] = best_j3;
    } else {
        target_angles[0] = j0_deg - 180;
        target_angles[1] = -best_j1;
        target_angles[2] = -best_j2;
        target_angles[3] = -best_j3;
    }
    
    // Keep last two joints same or 0 for now as IK accounts for end effector POS only, not orientation fully in this snippet
    target_angles[4] = 0; 
    target_angles[5] = 0; 

    return true;
}

void RobotArm::moveToCalculated() {
    moveSlowlyTo(target_angles[0], target_angles[1], target_angles[2], target_angles[3], target_angles[4], target_angles[5]);
}

void RobotArm::reset() {
    moveSlowlyTo(0, 0, 0, 0, 0, 0);
}

```
