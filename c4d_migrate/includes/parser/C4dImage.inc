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
    $pattern = '/\[c4d-image==([^=]*)==([0-9]+)x([0-9]+)==([^=]*)==([^=]*)==([^=]*)\]/iSu';
    preg_match($pattern, $tag, $parts);

    // Check tag.
    if (!$parts) {
      return FALSE;
    }

    // Fill data into the data array.
    $source = $this->cleanUpSource($parts[1]);
    $data['type']    .= ':' . $this->getOrigin($source);
    $data['source']   = $source;
    $data['width']    = (int) $parts[2];
    $data['height']   = (int) $parts[3];
    $data['alt']      = $parts[4];
    $data['title']    = $parts[5];
    $data['position'] = $parts[6];

    return $data;
  }

  /**
   * Get the origin from the source string.
   *
   * @param string $source
   *
   * @return string
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
    // Private file system.
    $pattern = '/^\.\.\/\.\.\/system\/files\//iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, 'sites/default/files/', $source);
    }

    $pattern = '/^\//iSu';
    if (1 === preg_match($pattern, $source)) {
      $source = preg_replace($pattern, NULL, $source);
    }

    return $source;
  }
}
