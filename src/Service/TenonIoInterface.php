<?php

namespace Drupal\tenon_io\Service;

/**
 * Interface for the TenonIo service.
 */
interface TenonIoInterface {

  /**
   * Retrieves the issues count of an URL from the cache.
   *
   * @param string $url
   *   The URL to retreive the issues count for.
   *
   * @return int
   *   The issues count for the given URL if known.
   */
  public function getIssuesCountFromCache($url);

  /**
   * Saves the issues count of an URL into the cache.
   *
   * @param string $url
   *   The URL to store the issues count for.
   * @param int $issuesCount
   *   The issues count.
   */
  public function setIssuesCountToCache($url, $issuesCount);

  /**
   * Sends a request to Tenon.io to check the given URL.
   *
   * @param string $url
   *   The URL to check for accessibility.
   *
   * @return array
   *   The API results.
   */
  public function checkPage($url);

}
