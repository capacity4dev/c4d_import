<?php
/**
 * @file
 * Log to watchdog.
 */

/**
 * This is a wrapper around the watchdog() functionality.
 */
class C4dMigrateLoggerWatchdog implements C4dMigrateLoggerInterface {
  /**
   * {@inheritdoc}
   */
  public function log($message, $variables = array(), $severity = self::INFO) {
    // We do not keep debug data in the watchdog table.
    if ($severity > self::INFO) {
      return;
    }

    watchdog('c4d_migrate', $message, $variables, $severity);
  }

}
