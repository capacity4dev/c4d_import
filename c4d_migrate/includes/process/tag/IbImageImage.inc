<?php
/**
 * @file
 * Processor for local ibimage:image tags.
 */

/**
 * Processor to make files out of ibimage:image tags.
 */
class C4dMigrateProcessTagIbImageImage extends C4dMigrateProcessTagIbImage {
  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag  = $this->getTag();
    $file = $this->getFile();

    $url = file_create_url($file->uri);

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<div class="ibimage %s ibimage-preset-%s">' .
      '<div class="ibimage-inner">' .
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="" />' .
      '<span class="caption">%s</span>' .
      '</div>' .
      '</div>'
      ,
      $tag['style'],
      $tag['preset'],
      $url,
      $file->fid,
      $tag['caption'],
      $tag['caption']
    );

    return $replacement;
  }
}
