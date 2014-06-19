<?php
/**
 * @file
 * Processor to identify all [video:…] tags.
 */

/**
 * Parser to identify and parse all [video:…] tags.
 *
 * This will parse out tags like:
 * <pre>
 * [video:http://www.youtube.com/v/FpX5x]
 * </pre>
 */
class C4dMigrateParserVideo extends C4dMigrateParserAbstract {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    $this->setPattern('/\[video:[^\]]*\]/iSu');
  }

  /**
   * {@inheritdoc}
   */
  protected function parseTag($tag) {
    $data = array(
      'type' => 'video',
      'tag'  => $tag,
      'url'  => NULL,
    );

    // Parse the tag.
    $parts = array();
    $pattern = '/\[video:([^\]]*)\]/iSu';
    preg_match($pattern, $tag, $parts);

    // Check tag.
    if (!$parts) {
      return FALSE;
    }

    // Fill data into the data array.
    $data['url'] = $parts[1];

    return $data;
  }
}
