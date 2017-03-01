<?php
/**
 * @file
 * Processor for local ibimage tags.
 */

/**
 * Processor to make files out of ibimage tags.
 */
abstract class C4dMigrateProcessTagIbImage extends C4dMigrateProcessTagAbstractFile {
  protected $source = NULL;

  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();

    $source = $this->getSource($tag['fid']);
    $file = $this->copyFile($source, 'public://media');

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
      '<img src="%s" class="img__fid__%d media-image" alt="%s" title="%s" />',
      $this->fileCreateUrl($new_image_url),
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
