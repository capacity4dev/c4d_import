<?php
/**
 * @file
 * Processor for local C4D-image tags.
 */

/**
 * Processor to make files out of c4d-image:local tags.
 *
 * Local configuration parameters:
 * - files_root : the root path where the source files are located.
 */
class C4dMigrateProcessTagEcMedia extends C4dMigrateProcessTagAbstractEmbed {
  /**
   * {@inheritdoc}
   */
  public function getReplacement() {
    $tag  = $this->getTag();
    $info = $this->getInfo();

    $replacement = $this->setDimensions(
      $info->code,
      $tag['width'],
      $tag['height']
    );

    return $replacement;
  }

  /**
   * Load the information from the ec_media_code table.
   *
   * @param array $tag
   *   The tag we need to load the info for.
   *
   * @return stdClass|bool
   *   The retrieved information or FALSE.
   */
  protected function loadInfo($tag) {
    // Load from the DB.
    $id = (int) $tag['id'];
    $q = $this->createSelect('ec_media_code', 'mc');
    $q->fields('mc');
    $q->condition('cid', $id, '=');
    $result = $q->execute();

    $info = $result->fetchObject();
    if (!$info) {
      $this->log(
        'No info available for ec_media with id @id (@tag).',
        array('@id' => $tag['id'], '@tag' => $tag['tag']),
        C4dMigrateLoggerInterface::ERROR
      );
      return FALSE;
    }

    return $info;
  }

}
