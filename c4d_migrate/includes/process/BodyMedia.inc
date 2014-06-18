<?php
/**
 * @file
 * Helper class to scan body content for media and import them to the media lib.
 */

/**
 * Helper class to convert old style media tags to new style media tags.
 */
class C4dMigrateProcessBodyMedia {
  /**
   * Names of all the available tag parsers.
   *
   * @var array
   */
  protected $parsers = array(
    'C4dMigrateParserC4dImage',
    'C4dMigrateParserIbImage',
    'C4dMigrateParserVideo',
    'C4dMigrateParserEcMedia',
  );

  /**
   * Mapping between type and the processor class.
   *
   * @var array
   */
  protected $processors = array(
    'c4d-image:local'  => 'C4dMigrateProcessTagC4dImageLocal',
    //'c4d-image:remote' => 'C4dMigrateProcessTagC4dImageLocal',
    //'ibimage'          => 'C4DMigrateProcessTagIbImage',
    //'video'            => 'C4DMigrateProcessTagVideo',
    //'ec_media'         => 'C4DMigrateProcessTagEcMedia',
  );


  /**
   * Process the body.
   */
  public function process($body) {
    $tags = array();
    $replacements = array();

    // Parse the tags out of the body field.
    foreach ($this->parsers as $parser_name) {
      $parser = new $parser_name();
      /* @var $parser C4dMigrateParserAbstract */
      if ($parser->parse($body)) {
        $tags += $parser->getTags();
      }
    }

    // Process the tags to replacement strings.
    foreach ($tags as $tag) {
      $processor = $this->getProcessor($tag['type']);
      if ($processor->process($tag)) {
        $replacements[$tag] = $processor->getReplacement();
      }
    }

    return $this->replaceTags($body, $replacements);
  }

  /**
   * Replace all tags in the body with the replacements.
   *
   * @param string $body
   *   The body where the tags needs to be replaced.
   * @param array $replacements
   *   The replacements array.
   *
   * @return string
   */
  protected function replaceTags($body, $replacements) {
    foreach($replacements as $search => $replace) {
      $body = str_replace($search, $replace, $body);
    }
    return $body;
  }

  /**
   * Get the processor object based on the tag type and mapping.
   *
   * @param string $type
   *
   * @return C4dMigrateProcessTagAbstract
   */
  protected function getProcessor($type) {
    if (!isset($this->processors[$type])) {
      return FALSE;
    }

    $processor_name = $this->processors[$type];
    return new $processor_name();
  }
}
