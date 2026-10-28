# Sensor-Filter

Header-only C++ sensor filters with fixed-size windows and no dynamic allocation: a moving average and a median filter. The median filter rejects single-sample sensor glitches that a moving average cannot.

## Requirements

| ID | Requirement |
|----|-------------|
| SF-001 | With no samples received, a filter shall report no value rather than a made-up number. |
| SF-002 | Before the window fills, the moving average shall use only the samples received so far. |
| SF-003 | The window shall hold at most N samples; each new sample beyond N shall replace the oldest. |
| SF-004 | The median filter shall return the middle sample (the lower middle for an even count), always a real sample. |
| SF-005 | The median filter shall ignore a single spike sample that pulls the moving average far off. |
| SF-006 | `reset` shall clear all samples. |

Window sizes of zero, and even sizes for the median filter, are rejected at compile time with `static_assert`.

Every check in `test_filters.cpp` is tagged with the requirement it verifies.

## Build and test

    make test        # build with -Wall -Wextra -Wpedantic -Werror and run the tests
    make coverage    # rebuild with gcov instrumentation and print coverage for filters.hpp

## Results

- 40 checks, 0 failures
- gcov on `filters.hpp` (each template instantiation counted separately): 100% of lines, 94% of branches taken at least once
