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
    // We do not keep informational & debug data in the message table.
    if ($severity > self::NOTICE) {
      return;
    }

    if (!class_exists('Migration')
      || !($migration = Migration::currentMigration())
    ) {
      return;
    }

    $log_entry = array(
      'message'   => $message,
      'variables' => $variables,
    );

    // Translate the severity codes.
    switch ($severity) {
      case self::ERROR:
        $migrate_severity = MigrationBase::MESSAGE_ERROR;
        break;
      case self::WARNING:
        $migrate_severity = MigrationBase::MESSAGE_WARNING;
        break;
      case self::NOTICE:
        $migrate_severity = MigrationBase::MESSAGE_NOTICE;
        break;

      case self::INFO:
      case self::DEBUG:
      default:
        $migrate_severity = MigrationBase::MESSAGE_INFORMATIONAL;
        break;
    }

    $migration->queueMessage(t($message, $variables), $migrate_severity);
  }
}
