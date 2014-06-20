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
class C4dMigrateProcessTagC4dImageRemote extends C4dMigrateProcessTagAbstractFile {
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
  public function getReplacement() {
    $tag  = $this->getTag();
    $file = $this->getFile();

    $replacement = sprintf(
      '<img src="%s" class="img__fid__%d media-image" style="height:%dpx; width:%dpx" alt="%s" title="%s" />',
      file_create_url($file->uri),
      $file->fid,
      $tag['height'],
      $tag['width'],
      $tag['alt'],
      $tag['title']
    );

    return $replacement;
  }

  /**
   * {@inheritdoc}
   */
  protected function copyFile($source, $target_folder) {
    try {
      $file_stream = file_get_contents($source);
    }
    catch (Exception $e) {
      return FALSE;
    }

    // We can only migrate files that exist.
    if (empty($file_stream)) {
      return FALSE;
    }

    $source_name = drupal_basename($source);

    // Make sure that the target folder exists and is writable.
    file_prepare_directory($target_folder, FILE_CREATE_DIRECTORY);

    $target_path = $target_folder . '/' . $source_name;

    $file = file_save_data($file_stream, $target_path);

    return ($file) ? $file : FALSE;
  }
}
