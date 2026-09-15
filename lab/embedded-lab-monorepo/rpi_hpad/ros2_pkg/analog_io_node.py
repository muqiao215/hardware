from __future__ import annotations

import argparse
import json
import time

from app.common import add_backend_args, replay_loop_enabled
from app.factory import create_board
from services.analog_service import AnalogService

try:
    import rclpy
    from rclpy.node import Node
    from std_msgs.msg import Float32

    ROS2_AVAILABLE = True
except ImportError:
    ROS2_AVAILABLE = False
    rclpy = None
    Node = object
    Float32 = object


class _AnalogIONode(Node):  # type: ignore[misc]
    def __init__(self, backend: str, csv_path: str | None = None) -> None:
        super().__init__("analog_io_node")
        self.board = create_board(backend, csv_path)
        self.service = AnalogService(self.board)
        self.pub0 = self.create_publisher(Float32, "/adc/ch0", 10)
        self.pub1 = self.create_publisher(Float32, "/adc/ch1", 10)
        self.create_timer(0.2, self._publish_samples)

    def _publish_samples(self) -> None:
        msg0 = Float32()
        msg1 = Float32()
        msg0.data = float(self.service.read_calibrated_voltage(0, 4))
        msg1.data = float(self.service.read_calibrated_voltage(1, 4))
        self.pub0.publish(msg0)
        self.pub1.publish(msg1)


def main() -> int:
    parser = argparse.ArgumentParser(description="ROS2 analog node with mock/replay fallback.")
    add_backend_args(parser)
    parser.add_argument("--samples", type=int, default=5)
    args = parser.parse_args()

    if not ROS2_AVAILABLE:
        board = create_board(
            args.backend,
            csv_path=args.csv,
            config_path=args.config,
            replay_loop=replay_loop_enabled(args.replay_loop),
        )
        service = AnalogService(board)
        print("rclpy not available; running fallback console publisher")
        for _ in range(args.samples):
            print(json.dumps(service.get_snapshot([0, 1])))
            time.sleep(0.2)
        return 0

    rclpy.init()
    node = _AnalogIONode(args.backend, args.csv)
    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
