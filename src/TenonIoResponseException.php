<?php

namespace Drupal\tenon_io;

/**
 * TenonIo exception class.
 */
class TenonIoResponseException extends \Exception {

  /**
   * The message type.
   *
   * @var string
   */
  protected $messageType;

  /**
   * The more info links.
   *
   * @var array
   */
  protected $moreInfo;

  /**
   * TenonIoResponseException constructor.
   *
   * @param string $message
   *   The message.
   * @param string $messageType
   *   The message type.
   * @param array $moreInfo
   *   The more info links array. Each link being defined by an array using the
   *   'title' and 'url' keys.
   * @param int $code
   *   The error code.
   * @param \Exception $previous
   *   The previous exception.
   */
  public function __construct($message = '', $messageType = '', array $moreInfo = [], $code = 0, \Exception $previous = NULL) {
    parent::__construct($message, $code, $previous);
    $this->messageType = $messageType;
    $this->moreInfo = $moreInfo;
  }

  /**
   * Gets the message type.
   *
   * @return string
   *   The message type.
   */
  public function getMessageType() {
    return $this->messageType;
  }

  /**
   * Gets the more info links.
   *
   * @return array
   *   The more info links array.
   */
  public function getMoreInfo() {
    return $this->moreInfo;
  }

}
