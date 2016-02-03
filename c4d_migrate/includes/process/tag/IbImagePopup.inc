<?php
/**
 * @file
 * Processor for local ibimage:popup tags.
 */

/**
 * Processor to make files out of ibimage:popup tags.
 */
class C4dMigrateProcessTagIbImagePopup extends C4dMigrateProcessTagIbImage {
  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag  = $this->getTag();
    $file = $this->getFile();

    $url = $this->fileCreateUrl($file->uri);

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<div class="ibimage %s ibimage-preset-%s">
        <div class="ibimage-inner">
          <a href="%s"%s class="ibimage-link popup lightbox">
            <img src="%s" class="img__fid__%d media-image" alt="%s" title="" />
            <span class="caption">%s</span>
          </a>
        </div>
      </div>',
      $tag['style'],
      $tag['preset'],
      $url,
      $tag['target'] == '_blank' ? ' target="_blank"' : '',
      $url,
      $file->fid,
      $tag['caption'],
      $tag['caption']
    );

    return $replacement;
  }

}
