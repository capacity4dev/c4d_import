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

    $string = array(
      '<img',
      'src="%s"',
      'class="img__fid__%d media-image"',
    );
    $args = array(
      $this->fileCreateUrl($file->uri),
      $file->fid,
    );

    if ($tag['width']) {
      $string[] = 'width="%d"';
      $args[] = $tag['width'];
    }
    if ($tag['height']) {
      $string[] = 'height="%d"';
      $args[] = $tag['height'];
    }
    if ($tag['alt']) {
      $string[] = 'alt="%s"';
      $args[] = $tag['alt'];
    }
    if ($tag['title']) {
      $string[] = 'title="%s"';
      $args[] = $tag['title'];
    }

    $string[] = '/>';

    $replacement = vsprintf(
      implode(' ', $string),
      $args
    );
    return $replacement;
  }

}
