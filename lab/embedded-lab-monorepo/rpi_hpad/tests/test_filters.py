from __future__ import annotations

import unittest

from services.filters import median_filter, moving_average, reject_outliers


class FiltersTest(unittest.TestCase):
    def test_moving_average(self) -> None:
        self.assertAlmostEqual(moving_average([1.0, 2.0, 3.0]), 2.0)

    def test_median_filter_even(self) -> None:
        self.assertAlmostEqual(median_filter([4.0, 1.0, 3.0, 2.0]), 2.5)

    def test_reject_outliers(self) -> None:
        self.assertEqual(reject_outliers([0.1, 2.0, 6.0], 0.0, 5.0), [0.1, 2.0])
