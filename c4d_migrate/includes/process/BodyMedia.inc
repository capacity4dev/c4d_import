<?php
/**
 * @file
 * Helper class to scan body content for media and import them to the media lib.
 */

/**
 * Helper class to convert old style media tags to new style media tags.
 */
class C4dMigrateProcessBodyMedia extends C4dMigrateProcessAbstract {
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
    'c4d-image:local'    => 'C4dMigrateProcessTagC4dImageLocal',
    'c4d-image:remote'   => 'C4dMigrateProcessTagC4dImageRemote',
    'ibimage:image'      => 'C4DMigrateProcessTagIbImageImage',
    'ibimage:link'       => 'C4DMigrateProcessTagIbImageLink',
    'ibimage:popup'      => 'C4DMigrateProcessTagIbImagePopup',
    'ibimage:photoalbum' => 'C4DMigrateProcessTagIbImagePhotoalbum',
    'ibimage:slideshow'  => 'C4DMigrateProcessTagIbImageSlideshow',
    'video'              => 'C4DMigrateProcessTagVideo',
    'ec_media'           => 'C4DMigrateProcessTagEcMedia',
  );

  /**
   * Process the body.
   */
  public function process($body) {
    $tags         = array();
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
      $processor = $this->getProcessor($tag);
      if (!$processor) {
        $replacements[$tag['tag']] = $this->createCommentTag($tag['tag']);
        continue;
      }

      $processor->setLoggers($this->getLoggers());
      if ($processor->process()) {
        $replacements[$tag['tag']] = $processor->getReplacement();
      }
      else {
        $replacements[$tag['tag']] = $this->createCommentTag($tag['tag']);
      }
    }

    return $this->replaceTags($body, $replacements);
  }

  /**
   * Replace all tags in the body with the replacements.
   *
   * @param string $body
   *   The body where the tags needs to be replaced.
   * @param array  $replacements
   *   The replacements array.
   *
   * @return string
   *   String with replacements.
   */
  protected function replaceTags($body, $replacements) {
    foreach ($replacements as $search => $replace) {
      $body = str_replace($search, $replace, $body);
    }

    $severity = C4dMigrateLoggerInterface::DEBUG;
    if ($count = count($replacements)) {
      $severity = C4dMigrateLoggerInterface::INFO;
    }
    $this->log(
      'Process Body Media : Replaced @count tags.',
      array('@count' => count($replacements)),
      $severity
    );


    return $body;
  }

  /**
   * Get the processor object based on the tag type and mapping.
   *
   * @param array $tag
   *   The tag to process
   *
   * @return C4dMigrateProcessTagAbstract
   *   Instanciated processor object.
   */
  protected function getProcessor($tag) {
    $type = $tag['type'];
    if (!isset($this->processors[$type])) {
      $this->log(
        'Unknown processor for tag type "@type"',
        array('@type' => $type),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    $processor_name = $this->processors[$type];

    return new $processor_name($tag, $this->getConfig());
  }

  /**
   * Create the config array.
   *
   * @return array
   */
  protected function getConfig() {
    static $config;

    if (!$config) {
      $config = array(
        'files_root' => c4d_migrate_get_files_root(),
        'db'         => c4d_migrate_get_db_connection(),
      );
    }

    return $config;
  }

  /**
   * Create a "hidden" tag for tags that were not replaced because of errors.
   *
   * @param string $tag
   *   HTML tag to put inside comment.
   *
   * @return string
   *   Commented tag.
   */
  protected function createCommentTag($tag) {
    return '<!--' . $tag . '-->';
  }
}
