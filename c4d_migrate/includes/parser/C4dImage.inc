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
    $data['source']   = $parts[1];
    $data['width']    = (int) $parts[2];
    $data['height']   = (int) $parts[3];
    $data['alt']      = $parts[4];
    $data['title']    = $parts[5];
    $data['position'] = $parts[6];

    return $data;
  }
}
