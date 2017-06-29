#!/usr/bin/awk -f

/^TEST_SUITE:/ { test_suite = $2; }

/^TEST_NUMBER:/ { test_number = $2; }

/^RESULT:/ { print test_suite, test_number, $0 }
