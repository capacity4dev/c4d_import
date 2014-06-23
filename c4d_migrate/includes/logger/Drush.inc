<?php
/**
 * @file
 * Log messages to the screen trough Drush..
 */

/**
 * Logger to output message to the screen.
 */
class C4dMigrateLoggerDrush implements C4dMigrateLoggerInterface {
  /**
   * {@inheritdoc}
   */
  public function log($message, $variables = array(), $severity = self::INFO) {
    // Check if Drush is available.
    if (!function_exists('drush_log')) {
      return;
    }

    // Translate the severity.
    switch ($severity) {
      case self::ERROR:
        $type = 'error';
        break;
      case self::WARNING:
        $type = 'warning';
        break;
      case self::NOTICE:
        $type = 'notice';
        break;
      case self::INFO:
        $type = 'info';
        break;
      case self::DEBUG:
        $type = 'debug';
        break;
    }

    drush_log(t($message, $variables), $type);
  }
}