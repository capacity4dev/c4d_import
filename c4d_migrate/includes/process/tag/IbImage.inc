<?php
/**
 * @file
 * Processor for local C4D-image tags.
 */

/**
 * Processor to make files out of ibimage tags.
 */
class C4dMigrateProcessTagIbImage extends C4dMigrateProcessTagAbstract {
  private $source = NULL;
  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();

    $file = $this->copyFile($this->getSource($tag['fid']), 'public://media');

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

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="%s" />',
      file_create_url($file->uri),
      $file->fid,
      $tag['caption'],
      $tag['caption']
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
    $q = $this->createSelect('files', 'f');
    $q->fields('f');
    $q->condition('fid', $fid, '=');
    $res = $q->execute();
    return $res->fetchObject();
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
    if (empty($this->source)) {
      $img = $this->getImageFile($fid);
      // $config = array('files_root' =>
      // '/Volumes/webdev/www/amplexor/capacity4dev.eu/trunk/platform/http');
      $this->source = $img->filepath;
    }
    return $this->source;
  }

  /**
   * Setter for $this->source.
   *
   * @param string $source
   *   File path.
   */
  public function setSource($source) {
    $this->source = $source;
  }
}
