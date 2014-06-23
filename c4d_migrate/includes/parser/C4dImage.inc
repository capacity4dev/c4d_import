<?php
/**
 * @file
 * Processor to identify all [c4d-image==] tags.
 */

/**
 * Parser to identify and parse all [c4d-image==] tags.
 *
 * This will parse out tags like:
 * <pre>
 * [c4d-image==/path/to/source-file.png==widthxheight==alt==title==position]
 * </pre>
 */
class C4dMigrateParserC4dImage extends C4dMigrateParserAbstract {
  /**
   * Source origins.
   *
   * @var string
   */
  const SOURCE_ORIGIN_LOCAL = 'local';
  const SOURCE_ORIGIN_REMOTE = 'remote';


  /**
   * {@inheritdoc}
   */
  public function __construct() {
    $this->setPattern('/\[c4d-image==[^\]]*\]/iSu');
  }

  /**
   * {@inheritdoc}
   */
  protected function parseTag($tag) {
    $data = array(
      'type'     => 'c4d-image',
      'tag'      => $tag,
      'source'   => NULL,
      'width'    => NULL,
      'height'   => NULL,
      'alt'      => NULL,
      'title'    => NULL,
      'position' => NULL,
    );

    // Parse the tag.
    $parts = array();
    $pattern = '/\[c4d-image==([^=]*)==(([0-9]+)x([0-9]+))?==([^=]*)==([^=]*)==([^=]*)\]/iSu';
    preg_match($pattern, $tag, $parts);

    // Check tag.
    if (!$parts) {
      return FALSE;
    }

    // Fill data into the data array.
    $source = $this->cleanUpSource($parts[1]);
    $data['type']  .= ':' . $this->getOrigin($source);
    $data['source'] = $source;
    if ($parts[3]) {
      $data['width'] = (int) $parts[3];
    }
    if ($parts[4]) {
      $data['height'] = (int) $parts[4];
    }
    if ($parts[5]) {
      $data['alt'] = $parts[5];
    }
    if ($parts[6]) {
      $data['title'] = $parts[6];
    }
    if ($parts[7]) {
      $data['position'] = $parts[7];
    }

    return $data;
  }

  /**
   * Get the origin from the source string.
   *
   * @param string $source
   *   Source path of the image.
   *
   * @return string
   *   String indicating remote or local file.
   */
  protected function getOrigin($source) {
    if (preg_match('|^http[s]?://|iSu', $source)) {
      return self::SOURCE_ORIGIN_REMOTE;
    }

    return self::SOURCE_ORIGIN_LOCAL;
  }

  /**
   * Cleanup the source URL.
   *
   * @param string $source
   *   The source URL.
   *
   * @return string
   *   The cleaned up version of the string.
   */
  protected function cleanUpSource($source) {
    // Relative path.
    $pattern = '/^(\.\.\/)+/iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, NULL, $source);
    }

    // PURL paths.
    $pattern = '/^\/[^\/]*\/system\/files\//iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, 'sites/default/files/', $source);
    }

    // Private file system.
    $pattern = '/^(\/)?system\/files\//iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, 'sites/default/files/', $source);
    }

    // Remove first /.
    $pattern = '/^\//iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, NULL, $source);
    }

    return $source;
  }
}
