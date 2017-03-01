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
    $tag = $this->getTag();
    $file = $this->getFile();

    $image_style = image_style_load($tag['preset']);

    $url = ($image_style) ? image_style_path(
      $image_style['name'],
      $file->uri
    ) : image_style_path('medium', $file->uri);

    $file_exists = file_exists($url);

    if (!$file_exists) {
      image_style_create_derivative(
        $image_style,
        $file->uri,
        $url
      );
    }

    $new_image_url = file_create_url($url);

    // @TODO: Need to work on the output. Classes etc...
    $replacement = sprintf(
      '<div class="ibimage %s ibimage-preset-%s">
        <div class="ibimage-inner">
          <img src="%s" class="img__fid__%d media-image" alt="%s" title="" />
          <span class="caption">%s</span>
        </div>
      </div>',
      $tag['style'],
      $tag['preset'],
      $this->fileCreateUrl($new_image_url),
      $file->fid,
      $tag['caption'],
      $tag['caption']
    );

    return $replacement;
  }

}
