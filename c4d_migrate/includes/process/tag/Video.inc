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
class C4dMigrateProcessTagVideo extends C4dMigrateProcessTagAbstract {
  /**
   * {@inheritdoc}
   */
  public function process() {
    // no processing needed :)
    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag  = $this->getTag();

    $replacement = sprintf(
      '<iframe src="%s?autoplay=0" width="520" height="450" frameborder="0"></iframe>',
      $tag['url']
    );
    return $replacement;
  }
}
