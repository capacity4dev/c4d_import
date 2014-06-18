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
abstract class C4dMigrateProcessTagAbstract {
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
   * The result of the file processing is a file in the file + file_entity tables.
   *
   * @var stdClass
   */
  private $file;


  /**
   * Create the processor by passing the tag data.
   *
   * @param array $tag
   * @param array $config
   */
  public function __construct(array $tag, array $config = array()) {
    $this->tag = $tag;
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
   * Set the file.
   *
   * @param stdClass
   *   The file object (result of the process).
   */
  protected function setFile(stdClass $file) {
    $this->file = $file;
  }

  /**
   * Get the file.
   *
   * @return stdClass
   *   The file object as saved in the local database.
   */
   public function getFile() {
     if (is_null($this->file)) {
       return FALSE;
     }

     return $this->file;
   }

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
   * @return mixed
   */
  protected function getConfigValue($key) {
    if (!array_key_exists($key, $this->config)) {
      return;
    }

    return $this->config[$key];
  }
}