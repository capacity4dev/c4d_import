<?php
/**
 * @file
 * Base class to import OG content data from capacity4dev into capacity4more.
 */

/**
 * Base class to import OG content data from capacity4dev into capacity4more.
 */
abstract class C4dMigrateImportBaseNodeOG extends C4dMigrateImportBaseNode {
  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
    'C4dMigrateImportUsers',
    'C4dMigrateImportVocabOGCategories',
    'C4dMigrateImportVocabOGTags',
    'C4dMigrateCreateCSVTermTopic',
  );

  /**
   * Flag whether this content type has categories enabled.
   *
   * @var bool
   */
  private $hasCategories = FALSE;
  /**
   * Flag whether this content type has tags enabled.
   *
   * @var bool
   */
  private $hasTags = FALSE;

  /**
   * Configure the migration class during creation.
   */
  public function __construct() {
    parent::__construct();

    $this->addFieldMapping(OG_AUDIENCE_FIELD, OG_AUDIENCE_FIELD);

    if ($this->hasCategories()) {
      $this->addFieldMapping('c4m_vocab_categories', 'categories')
           ->sourceMigration('C4dMigrateImportVocabOGCategories')
           ->arguments(array('source_type' => 'tid'));
    }
    if ($this->hasTags()) {
      $this->addFieldMapping('c4m_vocab_tags', 'tags')
           ->sourceMigration('C4dMigrateImportVocabOGTags')
           ->arguments(array('source_type' => 'tid'));
    }
  }

  /**
   * Prepare the row data loaded from the CSV.
   *
   * @param object $row
   *   The data from the CSV file.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $og_groups = array();

    $row->og_groups = unserialize($row->og_groups);

    foreach ($row->og_groups as $gid => $group_type) {
      $og_groups[] = $this->getMappedGroupGid($group_type, $gid);
    }

    $row->{OG_AUDIENCE_FIELD} = $og_groups;

    // @TODO: og_public?

    if ($this->hasCategories()) {
      $row->categories = unserialize($row->categories);
      $row->categories = $this->handleSourceMigration(
                              'C4dMigrateImportVocabOGCategories',
                                $row->categories,
                                NULL,
                                $this);
      // handleSourceMigration can return NULL, a single value or an array.
      // We need an array, even if it's empty.
      if (empty($row->categories)) {
        $row->categories = array();
      }
      if (!is_array($row->categories)) {
        $row->categories = array($row->categories);
      }
    }
    if ($this->hasTags()) {
      $row->tags = unserialize($row->tags);
      $row->tags = $this->handleSourceMigration(
                        'C4dMigrateImportVocabOGTags',
                          $row->tags,
                          NULL,
                          $this);
      // handleSourceMigration can return NULL, a single value or an array.
      // We need an array, even if it's empty.
      if (empty($row->tags)) {
        $row->tags = array();
      }
      if (!is_array($row->tags)) {
        $row->tags = array($row->tags);
      }
    }
    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $node
   *   The drupal node entity object.
   * @param object $row
   *   The source row object.
   */
  public function prepare($node, $row) {
    if (!$this->hasCategories()) {
      $row->categories = array();
    }
    if (!$this->hasTags()) {
      $row->tags = array();
    }

    $terms = array_merge($row->categories, $row->tags);
    if (!empty($terms)) {
      foreach ($terms as $key => $term) {
        $terms[$key] = array('target_id' => $term);
      }
      $node->og_vocabulary[LANGUAGE_NONE] = $terms;
    }
  }

  /**
   * Set status for categories and tags at once.
   *
   * Because categories and tags are mostly used together, this function lets
   * you set them both with one method.
   *
   * @param bool $status
   *   TRUE|FALSE flag for categories AND tags for this content type.
   */
  public function setCategoriesAndTags($status = FALSE) {
    $this->setCategories($status);
    $this->setTags($status);
  }

  /**
   * Set the status of categories.
   *
   * @param bool $status
   *   TRUE|FALSE flag for categories for this content type.
   */
  public function setCategories($status = FALSE) {
    $this->hasCategories = $status;
  }

  /**
   * Set the status of tags.
   *
   * @param bool $status
   *   TRUE|FALSE flag for tags for this content type.
   */
  public function setTags($status = FALSE) {
    $this->hasTags = $status;
  }

  /**
   * Retrieve the status of categories.
   *
   * @return bool
   *   TRUE|FALSE flag for categories for this content type.
   */
  public function hasCategories() {
    return $this->hasCategories;
  }

  /**
   * Retrieve the status of tags.
   *
   * @return bool
   *   TRUE|FALSE flag for tags for this content type.
   */
  public function hasTags() {
    return $this->hasTags;
  }

  /**
   * Actions after the data is saved in the database.
   *
   * @param object $entity
   *   The entity that is saved into the platform.
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($entity, $row) {
    parent::complete($entity, $row);

    // Make sure the messages have the proper timestamp.
    $this->messageInsertSave($entity, $row);

    // Make sure the update message also exists.
    $this->messageUpdateSave($entity, $row);
  }

}
