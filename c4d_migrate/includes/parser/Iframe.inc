<?php
/**
 * @file
 * Processor to identify all [ec_media…] tags.
 */

/**
 * Parser to identify and parse all <iframe></iframe> tags.
 */
class C4dMigrateParserIframe extends C4dMigrateParserAbstract {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    $this->setPattern('/<iframe[^>]*><\/iframe>/iSu');
  }

  /**
   * {@inheritdoc}
   */
  protected function parseTag($tag) {
    $data = array(
      'type'   => 'iframe',
      'tag'    => $tag,
      'src'    => $this->parseTagSource($tag),
      'width'  => $this->parseTagWidth($tag),
      'height' => $this->parseTagHeight($tag),
      'class'  => $this->parseTagClass($tag),
      'style'  => $this->parseTagStyle($tag),
    );

    return $data;
  }

  /**
   * Helper to parse the src from the iframe tag.
   *
   * @param string $tag
   *   The iframe tag.
   *
   * @return string|NULL
   *   The parsed source (src).
   */
  protected function parseTagSource($tag) {
    return $this->parseTagByPattern($tag, '/src="([^"]*)"/iSu');
  }

  /**
   * Helper to parse the width from the iframe tag.
   *
   * @param string $tag
   *   The iframe tag.
   *
   * @return int|NULL
   *   Width or null.
   */
  protected function parseTagWidth($tag) {
    $width = $this->parseTagByPattern($tag, '/width="([0-9]+)"/iSu');
    if ($width) {
      $width = (int) $width;
    }
    return $width;
  }

  /**
   * Helper to parse the height from the iframe tag.
   *
   * @param string $tag
   *   The iframe tag.
   *
   * @return int|NULL
   *   Height or null.
   */
  protected function parseTagHeight($tag) {
    $height = $this->parseTagByPattern($tag, '/height="([0-9]+)"/iSu');
    if ($height) {
      $height = (int) $height;
    }
    return $height;
  }

  /**
   * Helper to parse the class from the iframe tag.
   *
   * @param string $tag
   *   The iframe tag.
   *
   * @return string|NULL
   *   Class name or null.
   */
  protected function parseTagClass($tag) {
    return $this->parseTagByPattern($tag, '/class="([^"]*)"/iSu');
  }

  /**
   * Helper to parse the style from the iframe tag.
   *
   * @param string $tag
   *   The iframe tag.
   *
   * @return string|NULL
   *   Style or null.
   */
  protected function parseTagStyle($tag) {
    return $this->parseTagByPattern($tag, '/style="([^"]*)"/iSu');
  }

  /**
   * Helper to parse a single element from the tag by the given pattern.
   *
   * @param string $tag
   *   The iframe tag.
   * @param string $pattern
   *   The pattern to parse the value from the tag.
   *
   * @return string|NULL
   *   Tag by pattern or null.
   */
  protected function parseTagByPattern($tag, $pattern) {
    if (!preg_match($pattern, $tag, $result)) {
      return NULL;
    }
    return $result[1];
  }

}
