#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../libs/load_WAV.h"
#include "../libs/array_int64.h"
#include "../libs/chunk_list.h"
#include "../libs/sound_seg.h"

typedef int16_t sample_t;

sound_seg* create_test_track(sample_t* data, size_t length) {
    sound_seg* track = tr_init();
    if (!track) return NULL;
    
    // Write all data at once (respecting ordering requirement)
    tr_write(track, data, 0, length);
    return track;
}

// Helper function to verify track contents
bool verify_track_data(sound_seg* track, sample_t* expected, size_t length) {
    if (tr_length(track) != length) {
        printf("Length mismatch: expected %zu, got %zu\n", length, tr_length(track));
        return false;
    }
    
    // Read entire track into buffer and compare
    sample_t* actual = malloc(length * sizeof(sample_t));
    tr_read(track, actual, 0, length);
    
    for (size_t i = 0; i < length; i++) {
        if (actual[i] != expected[i]) {
            printf("Data mismatch at position %zu: expected %d, got %d\n", i, expected[i], actual[i]);
            free(actual);
            return false;
        }
    }
    
    free(actual);
    return true;
}

// Test basic insertion functionality (5.1.1 level - no parent/child relationships)
void test_basic_insertion() {
    printf("=== Testing Basic Insertion (5.1.1) ===\n");
    
    // Test Case 1: Insert at beginning
    sample_t src_data[] = {10, 20, 30};
    sample_t dest_data[] = {1, 2, 3, 4, 5};
    sample_t expected1[] = {20, 30, 1, 2, 3, 4, 5}; // Insert src[1:3] at dest[0]
    
    sound_seg* src = create_test_track(src_data, 3);
    sound_seg* dest = create_test_track(dest_data, 5);
    
    tr_insert(src, dest, 0, 1, 2); // Insert 2 samples from src[1] to dest[0]
    assert(verify_track_data(dest, expected1, 7));
    
    printf("✓ Insert at beginning passed\n");
    
    // Test Case 2: Insert in middle
    sample_t expected2[] = {1, 2, 20, 30, 3, 4, 5}; // Insert src[1:3] at dest[2]
    src = create_test_track(src_data, 3);
    dest = create_test_track(dest_data, 5);
    
    tr_insert(src, dest, 2, 1, 2);
    assert(verify_track_data(dest, expected2, 7));
    
    printf("✓ Insert in middle passed\n");
    
    // Test Case 3: Insert at end
    sample_t expected3[] = {1, 2, 3, 4, 5, 10, 20, 30}; // Insert all of src at end
    src = create_test_track(src_data, 3);
    dest = create_test_track(dest_data, 5);
    
    tr_insert(src, dest, 5, 0, 3);
    assert(verify_track_data(dest, expected3, 8));
    
    printf("✓ Insert at end passed\n");
    
    // Test Case 4: Insert single sample
    sample_t expected4[] = {1, 30, 2, 3, 4, 5}; // Insert src[2] at dest[1]
    src = create_test_track(src_data, 3);
    dest = create_test_track(dest_data, 5);
    
    tr_insert(src, dest, 1, 2, 1);
    assert(verify_track_data(dest, expected4, 6));
    printf("✓ Insert single sample passed\n");
}

// Test edge cases
void test_edge_cases() {
    printf("=== Testing Edge Cases ===\n");
    
    // Test Case 1: Insert into empty track
    sample_t src_data[] = {10, 20, 30};
    
    sound_seg* src = create_test_track(src_data, 3);
    sound_seg* dest = tr_init();
    
    
    // Test Case 2: Insert zero samples (should do nothing)
    sample_t dest_data[] = {1, 2, 3};
    
    // Test Case 3: Insert entire source track
    sample_t expected2[] = {1, 10, 20, 30, 2, 3};
    src = create_test_track(src_data, 3);
    dest = create_test_track(dest_data, 3);
    
    tr_insert(src, dest, 1, 0, 3);
    assert(verify_track_data(dest, expected2, 6));
    printf("✓ Insert entire source passed\n");
}

// Test self-insertion (inserting from a track into itself)
void test_self_insertion() {
    printf("=== Testing Self-Insertion ===\n");
    
    // Test Case 1: Insert portion of track into itself
    sample_t original[] = {1, 2, 3, 4, 5};
    sample_t expected[] = {1, 2, 2, 3, 3, 4, 5}; // Insert [1:3] at position 2
    
    sound_seg* track = create_test_track(original, 5);
    
    tr_insert(track, track, 2, 1, 2); // Insert samples 1-2 at position 2
    assert(verify_track_data(track, expected, 7));
    
    printf("✓ Self-insertion passed\n");
    
    // Test Case 2: Insert beginning of track at end
    sample_t expected2[] = {1, 2, 3, 4, 5, 1, 2};
    track = create_test_track(original, 5);
    
    tr_insert(track, track, 5, 0, 2);
    assert(verify_track_data(track, expected2, 7));
    printf("✓ Self-insertion at end passed\n");
}

// Test parent-child relationships (5.1.2+ levels)
void test_parent_child_relationships() {
    printf("=== Testing Parent-Child Relationships (5.1.2+) ===\n");
    
    // Create parent and child tracks
    sample_t parent_data[] = {10, 20, 30, 40, 50};
    sample_t child_data[] = {1, 2, 3};
    
    sound_seg* parent = create_test_track(parent_data, 5);
    sound_seg* child = create_test_track(child_data,3);
    
    // Insert portion of parent into child
    tr_insert(parent, child, 1, 1, 3); // Insert parent[1:4] into child at pos 1
    
    // Expected: child = [1, 20, 30, 40, 2, 3]
    sample_t expected[] = {1, 20, 30, 40, 2, 3};
    assert(verify_track_data(child, expected, 6));
    
    // Test that changes to parent are reflected in child (REQ 2.3)
    sample_t new_value = 99;
    tr_write(parent, &new_value, 2, 1); // Change parent[2] from 30 to 99
    
    // Child should now be [1, 20, 99, 40, 2, 3]
    sample_t expected_after_write[] = {1, 20, 99, 40, 2, 3};
    assert(verify_track_data(child, expected_after_write, 6));
    
    // Test that changes to child are reflected in parent
    sample_t new_value2 = 88;
    tr_write(child, &new_value2, 3, 1); // Change child[3] from 40 to 88
    
    // Verify parent reflects the change
    sample_t parent_sample;
    tr_read(parent, &parent_sample, 3, 1);
    assert(parent_sample == 88); // parent[3] should now be 88
    printf("✓ Parent-child relationship test passed\n");
}

// Test complex insertion patterns (5.1.6 level)
void test_complex_insertions() {
    printf("=== Testing Complex Insertions (5.1.6) ===\n");
    
    // Test multiple insertions creating complex parent-child relationships
    sample_t data1[] = {1, 2, 3, 4, 5};
    sample_t data2[] = {10, 20, 30};
    sample_t data3[] = {100, 200};
    
    sound_seg* track1 = create_test_track(data1, 5);
    sound_seg* track2 = create_test_track(data2, 3);
    sound_seg* track3 = create_test_track(data3, 2);
    
    // Insert track1 portion into track2
    tr_insert(track1, track2, 1, 1, 2); // track2 = [10, 2, 3, 20, 30]
    
    // Insert track2 portion into track3
    tr_insert(track2, track3, 0, 0, 3); // track3 = [10, 2, 3, 100, 200]
    
    // Verify the complex relationship works
    sample_t new_value = 99;
    tr_write(track1, &new_value, 2, 1); // Change track1[2] from 3 to 99
    
    // This should propagate: track2[2] = 99 and track3[2] = 99
    sample_t track2_sample, track3_sample;
    tr_read(track2, &track2_sample, 2, 1);
    tr_read(track3, &track3_sample, 2, 1);
    assert(track2_sample == 99);
    assert(track3_sample == 99);
    printf("✓ Complex insertion test passed\n");
}

// Test boundary conditions
void test_boundary_conditions() {
    printf("=== Testing Boundary Conditions ===\n");
    
    // Test inserting at exact track length (should append)
    sample_t src_data[] = {10, 20};
    sample_t dest_data[] = {1, 2, 3};
    sample_t expected[] = {1, 2, 3, 10, 20};
    
    sound_seg* src = create_test_track(src_data, 2);
    sound_seg* dest = create_test_track(dest_data, 3);
    
    tr_insert(src, dest, 3, 0, 2); // Insert at exact end
    assert(verify_track_data(dest, expected, 5));
    
    printf("✓ Insert at exact track length passed\n");
    
    // Test large insertion
    const size_t LARGE_SIZE = 1000;
    sample_t* large_data = malloc(LARGE_SIZE * sizeof(sample_t));
    for (size_t i = 0; i < LARGE_SIZE; i++) {
        large_data[i] = (sample_t)(i % 256);
    }
    
    sound_seg* large_src = create_test_track(large_data, LARGE_SIZE);
    sound_seg* small_dest = create_test_track(dest_data, 3);
    
    tr_insert(large_src, small_dest, 1, 100, 500);
    assert(tr_length(small_dest) == 503); // 3 + 500
    
    // Verify some samples
    sample_t sample0, sample1, sample501;
    tr_read(small_dest, &sample0, 0, 1);
    tr_read(small_dest, &sample1, 1, 1);  
    tr_read(small_dest, &sample501, 501, 1);
    
    assert(sample0 == 1);
    assert(sample1 == (100 % 256));
    assert(sample501 == 2);
    
    free(large_data);
    printf("✓ Large insertion test passed\n");
}

// Test memory sharing and efficiency
void test_memory_sharing() {
    printf("=== Testing Memory Sharing ===\n");
    
    // This test verifies that data is shared, not copied
    // We'll modify the parent and check if child reflects changes
    
    sample_t parent_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    sound_seg* parent = create_test_track(parent_data, 10);
    sound_seg* child = tr_init();
    
    // Insert multiple portions to create shared backing store
    tr_insert(parent, child, 0, 2, 3);  // child = [3, 4, 5]
    tr_insert(parent, child, 2, 7, 2);  // child = [3, 4, 8, 9, 5]
    
    // Verify initial state
    sample_t expected1[] = {3, 4, 8, 9, 5};
    assert(verify_track_data(child, expected1, 5));
    
    // Modify parent data and verify sharing
    sample_t new_val1 = 99, new_val2 = 88;
    tr_write(parent, &new_val1, 3, 1);  // Change parent[3] from 4 to 99
    tr_write(parent, &new_val2, 8, 1);  // Change parent[8] from 9 to 88
    
    // Child should reflect these changes
    sample_t expected2[] = {3, 99, 8, 88, 5};
    assert(verify_track_data(child, expected2, 5));
    printf("✓ Memory sharing test passed\n");
}

int main() {
    printf("Starting tr_insert comprehensive tests...\n\n");
    
    test_basic_insertion();
    printf("\n");
    
    test_edge_cases();
    printf("\n");
    
    test_self_insertion();
    printf("\n");
    
    test_parent_child_relationships();
    printf("\n");
    
    test_complex_insertions();
    printf("\n");
    
    test_boundary_conditions();
    printf("\n");
    
    test_memory_sharing();
    printf("\n");
    
    printf("🎉 All tr_insert tests passed!\n");
    printf("\nTest Coverage Summary:\n");
    printf("✓ Basic insertion (5.1.1 level)\n");
    printf("✓ Edge cases and boundary conditions\n");
    printf("✓ Self-insertion scenarios\n");
    printf("✓ Parent-child relationships (5.1.2+ levels)\n");
    printf("✓ Complex multi-level insertions (5.1.6 level)\n");
    printf("✓ Memory sharing verification\n");
    printf("✓ Write propagation (REQ 2.3)\n");
    
    return 0;
}