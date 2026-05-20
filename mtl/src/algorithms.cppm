/**
 * @file mtl/src/algorithms.cppm
 * @brief Module partition that re-exports sorting algorithm modules.
 *
 * This umbrella module provides access to algorithm implementations such as
 * quick sort and merge sort by re-exporting their respective module partitions.
 */
export module mtl.algorithms;

export import mtl.core;
export import :quick_sort;
export import :merge_sort;
