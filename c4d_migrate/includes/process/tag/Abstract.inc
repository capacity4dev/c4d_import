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
   * The result of the processing is a file in the file + file_entity tables.
   *
   * @var stdClass
   */
  private $file;

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
   * Set the file.
   *
   * @param object $file
   *   The file object (result of the process).
   */
  protected function setFile(stdClass $file) {
    $this->file = $file;
  }

  /**
   * Get the file.
   *
   * @return object
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
   * @return string
   *   The configuration value.
   */
  protected function getConfigValue($key) {
    if (!array_key_exists($key, $this->config)) {
      return;
    }

    return $this->config[$key];
  }

  /**
   * Helper to copy a file from the old platform to the new.
   *
   * @param string $source
   *   The source path (including the filename) of the file that needs to be
   *   copied. The source path is relative to the files root of the old
   *   platform.
   * @param string $target_folder
   *   The target directory URI where the file should be copied to.
   *
   * @return stdClass|bool
   *   A file object if the copy was successful.
   *   FALSE if the file could not be copied.
   */
  protected function copyFile($source, $target_folder) {
    // Construct the full file path to the source file.
    $source_path = $this->getConfigValue('files_root')
      . '/'
      . $source;
    $source_name = drupal_basename($source_path);

    // We can only migrate files that exist.
    if (!file_exists($source_path)) {
      return FALSE;
    }

    // Make sure that the target folder exists and is writable.
    file_prepare_directory($target_folder, FILE_CREATE_DIRECTORY);

    // Create a fake file object so we can use the file_copy() function.
    $file_source           = new stdClass();
    $file_source->uri      = $source_path;
    $file_source->filename = $source_name;
    $file_source->filemime = file_get_mimetype($source_path);
    $file_source->status   = FILE_STATUS_PERMANENT;

    // Copy the file.
    $target_path = $target_folder . '/' . $source_name;
    $file        = file_copy($file_source, $target_path, FILE_EXISTS_RENAME);

    return ($file) ? $file : FALSE;
  }

  /**
   * Private helper function to inform drupal about our d6 connection.
   *
   * This will add the DB connection parameters to the DB connections list.
   */
  private function setUpConnection() {
    $existing = Database::getConnectionInfo(C4D_MIGRATE_DATABASE);
    if (!is_null($existing)) {
      // Already setup, don't do it again.
      return;
    }

    // Add information based on the variables.
    $c4d_database = array(
      'database' => variable_get(C4D_MIGRATE_DB_NAME, ''),
      'username' => variable_get(C4D_MIGRATE_DB_USER, ''),
      'password' => variable_get(C4D_MIGRATE_DB_PASS, ''),
      'host'     => variable_get(C4D_MIGRATE_DB_HOST, ''),
      'driver'   => 'mysql',
    );
    Database::addConnectionInfo(C4D_MIGRATE_DATABASE, 'default', $c4d_database);
  }

  /**
   * Get the connection to the D6 database.
   *
   * @return DatabaseConnection
   *   The Database connection object.
   *
   * @throws Exception
   */
  public function getConnection() {
    try {
      $this->setUpConnection();

      return Database::getConnection('default', C4D_MIGRATE_DATABASE);
    }
    catch (Exception $e) {
      drupal_set_message(
        t('Unable to connect to the database, <a href="@url">please check your settings</a>.',
          array(
            '@url' => url('admin/content/migrate/import'),
          )
        ),
        'error'
      );

      // Bubble up the exception.
      throw $e;
    }
  }

  /**
   * Set the right database and return a selectQuery.
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
    if ($conn = $this->getConnection()) {
      return $conn->select($name, $alias);
    }
  }
}
