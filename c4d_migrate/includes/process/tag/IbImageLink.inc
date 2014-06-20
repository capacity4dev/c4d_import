<?php
/**
 * @file
 * Processor for local ibimage:link tags.
 */

/**
 * Processor to make files out of ibimage:link tags.
 */
class C4dMigrateProcessTagIbImageLink extends C4dMigrateProcessTagIbImage {
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
      '<a href="%s"%s class="ibimage-link">' .
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="" />' .
      '<span class="caption">%s</span>' .
      '</a>' .
      '</div>' .
      '</div>'
      ,
      $tag['style'],
      $tag['preset'],
      $tag['link'],
      $tag['target'] == '_blank' ? ' target="_blank"' : '',
      $url,
      $file->fid,
      $tag['caption'],
      $tag['caption']
    );

    return $replacement;
  }
}
