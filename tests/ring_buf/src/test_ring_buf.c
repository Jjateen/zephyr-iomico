/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	rb_push(99);
	rb_init(4);
	zassert_true(rb_is_empty(), "After reinit buffer must be empty");
	zassert_equal(rb_count(), 0U, "After reinit count must be 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int v = 0;

	zassert_equal(rb_push(42), 0, "push(42) must return 0");
	zassert_equal(rb_pop(&v), 0, "pop must return 0");
	zassert_equal(v, 42, "popped value must be 42");
	zassert_true(rb_is_empty(), "buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int v = 0;

	zassert_equal(rb_push(1), 0, "push(1) must return 0");
	zassert_equal(rb_push(2), 0, "push(2) must return 0");
	zassert_equal(rb_push(3), 0, "push(3) must return 0");

	zassert_equal(rb_pop(&v), 0, "1st pop must return 0");
	zassert_equal(v, 1, "1st pop value must be 1");

	zassert_equal(rb_pop(&v), 0, "2nd pop must return 0");
	zassert_equal(v, 2, "2nd pop value must be 2");

	zassert_equal(rb_pop(&v), 0, "3rd pop must return 0");
	zassert_equal(v, 3, "3rd pop value must be 3");

	zassert_true(rb_is_empty(), "buffer must be empty after 3 pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	zassert_equal(rb_push(1), 0, "push 1");
	zassert_equal(rb_push(2), 0, "push 2");
	zassert_equal(rb_push(3), 0, "push 3");
	zassert_equal(rb_push(4), 0, "push 4");
	zassert_true(rb_is_full(), "buffer must be full");
	zassert_equal(rb_push(99), -ENOSPC, "push past capacity must return -ENOSPC");
	zassert_equal(rb_count(), 4U, "count must remain 4 after rejected push");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int v = 0;

	rb_push(7);

	zassert_equal(rb_peek(&v), 0, "1st peek must return 0");
	zassert_equal(v, 7, "1st peek value must be 7");

	v = 0;
	zassert_equal(rb_peek(&v), 0, "2nd peek must return 0");
	zassert_equal(v, 7, "2nd peek value must be 7");

	zassert_equal(rb_count(), 1U, "count must still be 1 after two peeks");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	zassert_equal(rb_push(1), 0, "push 1");
	zassert_equal(rb_push(2), 0, "push 2");
	zassert_equal(rb_push(3), 0, "push 3");
	zassert_equal(rb_push(4), 0, "push 4");
	zassert_true(rb_is_full(), "rb_is_full() must be true after 4 pushes");
	zassert_equal(rb_count(), 4U, "rb_count() must be 4");
}
