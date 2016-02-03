<?php
/**
 * @file
 * Processor for local C4D-image tags.
 */

/**
 * Processor to make files out of c4d-image:remote tags.
 *
 * Local configuration parameters:
 * - files_root : the root path where the source files are located.
 */
class C4dMigrateProcessTagC4dImageRemote extends C4dMigrateProcessTagC4dImageLocal {
  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag  = $this->getTag();
    $file = $this->copyFile($tag['source'], 'public://media');

    if (!$file) {
      return FALSE;
    }

    $this->setFile($file);

    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  protected function copyFile($source, $target_folder) {
    $source_name = drupal_basename($source);
    $existing = $this->loadFileFromMigrateMapping($source);
    if ($existing) {
      return $existing;
    }

    try {
      $file_stream = file_get_contents($source);
    }
    catch (Exception $e) {
      $this->log(
        'An exception occured while retrieving @source. Message: @message.',
        array('@source' => $source, '@message' => $e->getMessage()),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    // We can only migrate files that exist.
    if (empty($file_stream)) {
      $this->log(
        'The specified source file @source does not exist.',
        array('@source' => $source),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    // Make sure that the target folder exists and is writable.
    file_prepare_directory($target_folder, FILE_CREATE_DIRECTORY);

    $target_path = $target_folder . '/' . $this->safeFilename($source_name);

    $file = file_save_data($file_stream, $target_path);
    if (!$file) {
      $this->log(
        'The specified remote file @source could not be copied to @target.',
        array('@source' => $source, '@target' => $target_path),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    $this->saveMigrateMapping($source, $file);

    return $file;
  }

}
