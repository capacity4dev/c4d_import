<?php
/**
 * @file
 * Logger interface
 */

/**
 * Logger interface.
 */
interface C4dMigrateLoggerInterface {
  /**
   * Possible severity.
   */
  const ERROR = 3;
  const WARNING = 4;
  const NOTICE = 5;
  const INFO = 6;
  const DEBUG = 7;

  /**
   * Log a message.
   *
   * @param string $message
   *   The message to log.
   * @param array $variables
   *   The variables for the message.
   * @param int $severity
   *   The severity of the logged message.
   */
  public function log($message, $variables = array(), $severity = self::INFO);
}
