<?php
/**
 * @file
 * Base process class with support for loggers.
 */

/**
 * Base class for all processors.
 *
 * This provides the possibility to add loggers.
 */
abstract class C4dMigrateProcessAbstract {
  /**
   * The attached loggers.
   *
   * @var array
   */
  private $loggers = array();

  /**
   * Add a logger to the processor.
   *
   * @param C4dMigrateLoggerInterface $logger
   *   The logger to attach.
   */
  public function addLogger(C4dMigrateLoggerInterface $logger) {
    $name = get_class($logger);
    $this->loggers[$name] = $logger;
  }

  /**
   * Get all attached loggers.
   *
   * @return array
   *   Array of attached loggers.
   */
  public function getLoggers() {
    return $this->loggers;
  }

  /**
   * Set all loggers at once.
   *
   * @param array $loggers
   *   An array of C4dMigrateLoggerInterface loggers.
   */
  public function setLoggers($loggers) {
    foreach ($loggers as $logger) {
      $this->addLogger($logger);
    }
  }

  /**
   * Log a message.
   *
   * @param string $message
   *   The message to log.
   * @param array $variables
   *   The variables to us in the log message.
   * @param int $severity
   *   The severity of the message.
   */
  public function log(
    $message, $variables = array(), $severity = C4dMigrateLoggerInterface::INFO
  ) {
    foreach ($this->loggers as $logger) {
      $logger->log($message, $variables, $severity);
    }
  }

}
