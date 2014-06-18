<?php
/**
 * @file
 * Processor for local C4D-image tags.
 */

/**
 * Processor to make files out of c4d-image:local tags.
 *
 * Local configuration parameters:
 * - files_root : the root path where the source files are located.
 */
class C4dMigrateProcessTagC4dImageLocal extends C4dMigrateProcessTagAbstract {
  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();

    // Source file.
    $source_path = $this->getConfigValue('files_root')
      . '/'
      . $tag['source'];
    $source_name = drupal_basename($source_path);

    // We can only migrate files that exist.
    if (!file_exists($source_path)) {
      return FALSE;
    }

    // Copy the file to the new platform.
    $target_folder = 'public://media';
    $target_path   = $target_folder . '/' . $source_name;
    $file_source = new stdClass();
    $file_source->uri      = $source_path;
    $file_source->filename = $source_name;
    $file_source->filemime = file_get_mimetype($source_path);
    $file_source->status   = FILE_STATUS_PERMANENT;

    file_prepare_directory($target_folder, FILE_CREATE_DIRECTORY);
    $file = file_copy($file_source, $target_path, FILE_EXISTS_RENAME);
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
    $tag   = $this->getTag();
    $file  = $this->getFile();

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
}