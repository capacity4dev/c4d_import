<?php
/**
 * @file
 * Base tag processor.
 */

/**
 * Abstract tag processor to move files parsed from tags to the new platform.
 *
 * The source of the tags is converted to a media file (file_entity).
 */
abstract class C4dMigrateProcessTagAbstract extends C4dMigrateProcessAbstract {
  /**
   * The tag that needs to be processed.
   *
   * @var array
   */
  private $tag;

  /**
   * The configuration.
   *
   * @var array
   */
  private $config;

  /**
   * Create the processor by passing the tag data.
   *
   * @param array $tag
   *   The tag array to process.
   * @param array $config
   *   Config parameters for the processor.
   */
  public function __construct(array $tag, array $config = array()) {
    $this->tag    = $tag;
    $this->config = $config;
  }

  /**
   * Process the given tag.
   *
   * @return bool
   *   Success.
   */
  abstract public function process();

  /**
   * Get the replacement string we need to use in the body.
   *
   * @return string
   *   The string that will replace the original tag.
   */
  abstract public function getReplacement();

  /**
   * Get the tag we are processing.
   *
   * @return array
   *   The tag data.
   */
  public function getTag() {
    return $this->tag;
  }

  /**
   * Get a part of the configuration array.
   *
   * @param string $key
   *   The configuration key.
   *
   * @return string
   *   The configuration value.
   */
  protected function getConfigValue($key) {
    if (!array_key_exists($key, $this->config)) {
      return NULL;
    }

    return $this->config[$key];
  }

  /**
   * Create a selectQuery object using the db connection in the config.
   *
   * @param string $name
   *   Name of the database table.
   * @param string $alias
   *   Alias to give the database table.
   *
   * @return bool|SelectQueryInterface
   *   The select object.
   */
  public function createSelect($name, $alias) {
    if ($conn = $this->getConfigValue('db')) {
      return $conn->select($name, $alias);
    }
    return FALSE;
  }
}
