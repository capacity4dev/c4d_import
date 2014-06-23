<?php
/**
 * @file
 * Bridge to the migrate logger.
 */

/**
 * Logger that acts as a bridge to the migrate messages.
 */
class C4dMigrateLoggerMigrate implements C4dMigrateLoggerInterface {
  /**
   * {inheritdoc}
   */
  public function log($message, $variables = array(), $severity = self::INFO) {
    if (!function_exists('migrate_watchdog')) {
      return;
    }

    $log_entry = array(
      'message'   => $message,
      'variables' => $variables,
      'severity'  => $severity,
    );
    migrate_watchdog($log_entry);
  }
}
