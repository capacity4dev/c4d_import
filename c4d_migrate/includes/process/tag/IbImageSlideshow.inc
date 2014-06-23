<?php
/**
 * @file
 * Processor for local ibimage:slideshow tags.
 */

/**
 * Processor to make files out of ibimage:slideshow tags.
 */
class C4dMigrateProcessTagIbImageSlideshow extends C4dMigrateProcessTagIbImage {
  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag  = $this->getTag();
    $file = $this->getFile();

    $url = $this->fileCreateUrl($file->uri);

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<div class="ibimage %s ibimage-preset-%s">' .
      '<div class="ibimage-inner">' .
      '<a href="%s"%s class="ibimage-link photo-album-slideshow popup lightbox">' .
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="" />' .
      '<span class="caption">%s</span>' .
      '</a>' .
      '</div>' .
      '</div>'
      ,
      $tag['style'],
      $tag['preset'],
      $url, // @TODO LINK TO PHOTOALBUM!
      $tag['target'] == '_blank' ? ' target="_blank"' : '',
      $url,
      $file->fid,
      $tag['caption'],
      $tag['caption']
    );

    return $replacement;
  }
}
