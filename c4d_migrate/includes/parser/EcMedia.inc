<?php
/**
 * @file
 * Processor to identify all [ec_media…] tags.
 */

/**
 * Parser to identify and parse all [ec_media…] tags.
 *
 * This will parse out tags like:
 * <pre>
 * [ec_media:107 width:560 height:315 autoplay:0 align:center]
 * </pre>
 */
class C4dMigrateParserEcMedia extends C4dMigrateParserAbstract {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    $this->setPattern('/\[ec_media:[0-9]+[^\]]*\]/iSu');
  }

  /**
   * {@inheritdoc}
   */
  protected function parseTag($tag) {
    $data = array(
      'type'     => 'ec_media',
      'id'       => NULL,
      'width'    => NULL,
      'height'   => NULL,
      'autoplay' => NULL,
      'align'    => NULL,
    );

    // Parse the tag.
    $parts = array();
    $pattern = '/\[ec_media:([0-9]+) width:([0-9]+) height:([0-9]+) autoplay:([01]{1}) align:([^\]]*)\]/iSu';
    preg_match($pattern, $tag, $parts);

    // Check tag.
    if (!$parts) {
      return FALSE;
    }

    // Fill data into the data array.
    $data['id']       = (int)$parts[1];
    $data['width']    = (int)$parts[2];
    $data['height']   = (int)$parts[3];
    $data['autoplay'] = (int)$parts[4];
    $data['align']    = $parts[5];

    return $data;
  }
}