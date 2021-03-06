<?php
/**
 * @file
 * Processor to identify all [ibimage==] tags.
 */

/**
 * Parser to identify and parse all [ibimage==] tags.
 *
 * This will parse out tags like:
 * <pre>
 * [ibimage==FILE_ID==PRESET==LINK==LINK_TARGET==STYLE==CAPTION]
 * </pre>
 */
class C4dMigrateParserIbImage extends C4dMigrateParserAbstract {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    $this->setPattern('/\[ibimage==[^\]]*\]/iSu');
  }

  /**
   * {@inheritdoc}
   */
  protected function parseTag($tag) {
    $data = array(
      'type'    => 'ibimage',
      'tag'     => $tag,
      'fid'     => NULL,
      'preset'  => NULL,
      'link'    => NULL,
      'target'  => NULL,
      'style'   => NULL,
      'caption' => NULL,
    );

    // Parse the tag.
    $parts = array();
    $pattern = '/\[ibimage==([0-9]+)==([^=]*)==([^=]*)==([^=]*)==([^=]*)==([^=]*)\]/iSu';
    preg_match($pattern, $tag, $parts);

    // Check tag.
    if (!$parts) {
      return FALSE;
    }

    // Fill data into the data array.
    $data['type']    .= ':' . $this->getType($parts[3]);
    $data['fid']      = (int) $parts[1];
    $data['preset']   = $parts[2];
    $data['link']     = $parts[3];
    $data['target']   = $parts[4];
    $data['style']    = $parts[5];
    $data['caption']  = $parts[6];

    return $data;
  }

  /**
   * Map the tag link style to the right type of processor.
   *
   * @param string $link
   *   Link (type) of the ibimage tag.
   *
   * @return string
   *   Mapped processor for this tag.
   */
  private function getType($link) {
    switch ($link) {
      case 'photo_album_link':
        return 'photoalbum';

      case 'photo_album_slideshow':
        return 'slideshow';

      case 'popup':
        return 'popup';

      case 'none':
        return 'image';

      default:
        return 'link';
    }
  }

}
