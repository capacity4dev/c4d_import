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

    // Check if running Drush in verbose or debug mode.
    if (!drush_get_context('DRUSH_VERBOSE') && !drush_get_context('DRUSH_DEBUG')) {
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