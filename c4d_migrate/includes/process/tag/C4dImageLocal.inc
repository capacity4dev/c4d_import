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
class C4dMigrateProcessTagC4dImageLocal extends C4dMigrateProcessTagAbstractFile {
  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();
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
}
