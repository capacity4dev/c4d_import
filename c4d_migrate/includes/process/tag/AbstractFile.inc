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
abstract class C4dMigrateProcessTagAbstractFile extends C4dMigrateProcessTagAbstract {
  /**
   * The result of the processing is a file in the file + file_entity tables.
   *
   * @var stdClass
   */
  private $file;

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
      $this->log(
        'The specified source file @file does not exist.',
        array('@file' => $source_path),
        C4dMigrateLoggerInterface::ERROR
      );
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

    if (!$file) {
      $this->log(
        'The specified file @source could not be copied to @target.',
        array('@source' => $source_path, '@target' => $target_path),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    return $file;
  }

  /**
   * Helper to reduce the filename if to long.
   *
   * @param string $filename
   *   Filename we need to check.
   *
   * @return string
   *   The original if not to long or an MD5 hash of the original file.
   */
  protected function safeFilename($filename) {
    if (drupal_strlen($filename) < 200) {
      return $filename;
    }

    $path_info = pathinfo($filename);
    $checksum = md5($path_info['filename']);
    $safe = $checksum . '.' . $path_info['extension'];
    return $safe;
  }
}
