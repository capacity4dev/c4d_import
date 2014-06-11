<?php
/**
 * @file
 * Abstract class for text parsers.
 */

/**
 * Abstract class to parse text for tags.
 */
abstract class C4dMigrateParserAbstract {
  /**
   * The pattern we are searching for in the text.
   *
   * @var string
   */
  private $pattern;

  /**
   * The found tags and their parsed data.
   *
   * @var array
   */
  private $tags = array();


  /**
   * Class constructor.
   *
   * Use to define the pattern.
   */
  abstract public function __construct();

  /**
   * Parser to parse all the identified strings.
   *
   * @param string $text
   *   The text we need to parse.
   *
   * @return int
   *   The number of found tags.
   */
  public function parse($text) {
    $matches = array();
    if (!preg_match_all($this->getPattern(), $text, $matches)) {
      return 0;
    }

    foreach ($matches[0] AS $tag) {
      $data = $this->parseTag($tag);
      if (!$data) {
        continue;
      }

      $this->addTag($tag, $data);
    }

    return count($this->getTags());
  }

  /**
   * Parse a single tag into a data array.
   *
   * @param string $tag
   *   The tag to parse.
   *
   * @return array
   *   The parsed data.
   */
  abstract protected function parseTag($tag);

  /**
   * Get all parsed tags.
   *
   * @return array
   *   The tags found in the content.
   */
  public function getTags() {
    return $this->tags;
  }

  /**
   * Add a single tag and the parsed data to the found array.
   *
   * @param string $tag
   * @param array $data
   */
  private function addTag($tag, array $data) {
    $this->tags[$tag] = $data;
  }

  /**
   * Set the regular expression pattern to search for.
   *
   * @param string $pattern
   *   Regular expression.
   */
  protected function setPattern($pattern) {
    $this->pattern = $pattern;
  }

  /**
   * Get the regular expression pattern to search for.
   *
   * @return string
   *   Regular expression.
   */
  private function getPattern() {
    return $this->pattern;
  }
}