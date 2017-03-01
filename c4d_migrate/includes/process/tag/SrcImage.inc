<?php
/**
 * @file
 * Processor for local <img> tags.
 */

/**
 * Processor to make files out of <img> tags.
 */
class C4DMigrateProcessTagSrcImage extends C4dMigrateProcessTagAbstractFile {
  protected $source = NULL;

  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();

    // Manipulate the src tag. Retrieve everything after /system/files:
    $split_source = explode('/system/files', $tag['src']);
    $relative_path = $split_source[1];
    $file_path = 'sites/default/files' . $relative_path;

    $file = $this->copyFile($file_path, 'public://media');

    if (!$file) {
      // Try copying it manually.
      $file_copy = system_retrieve_file($file_path, 'public://media', TRUE);
      if (!$file_copy) {
        return FALSE;
      }

      $file = $file_copy;
    }

    $this->setFile($file);

    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag = $this->getTag();
    $file = $this->getFile();

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="%s" height="%s" width="%s" style="%s" />',
      $this->fileCreateUrl($file->uri),
      $file->fid,
      $tag['caption'],
      $tag['caption'],
      $tag['height'],
      $tag['width'],
      $tag['style']
    );

    return $replacement;
  }

  /**
   * Retrieve the file object related with the file id.
   *
   * @param int $fid
   *   File id to retrieve the file object.
   *
   * @return object|FALSE
   *   File object or FALSE if not found.
   */
  public function getImageFile($fid) {
    $q = $this->createSelect('c4d_export_other_files', 'f');
    $q->fields('f');
    $q->condition('fid', $fid, '=');
    $res = $q->execute();

    $file = $res->fetchObject();

    if (!$file) {
      $this->log(
        'Could not load the file information for id @id.',
        array('@id' => $fid),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    return $file;
  }

  /**
   * Retrieve the file, either by fid of the file or a defined source path.
   *
   * @param int $fid
   *   Optional file id.
   *
   * @return object
   *   File object.
   */
  public function getSource($fid) {
    $img = $this->getImageFile($fid);
    $this->source = $img->filepath;

    return $this->source;
  }

}
