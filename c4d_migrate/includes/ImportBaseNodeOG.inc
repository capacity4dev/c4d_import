<?php
/**
 * @file
 * Base class to import OG content data from capacity4dev into capacity4more.
 */

module_load_include('inc', 'c4d_migrate', 'c4d_migrate.converter');

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
   * Categories & Tags mapping array.
   *
   * @var array
   */
  private $mappingCategoriesAndTags = NULL;

  /**
   * Configure the migration class during creation.
   */
  public function __construct() {
    parent::__construct();

    $this->addFieldMapping(OG_AUDIENCE_FIELD, OG_AUDIENCE_FIELD);

    // Initialize the Categories & Tags mapping array.
    $this->getMappingCategoriesAndTags();

    $this->addFieldMapping('c4m_vocab_date', 'c4m_vocab_date');
    $this->addFieldMapping('c4m_vocab_date:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('c4m_vocab_document_type', 'c4m_vocab_document_type');
    $this->addFieldMapping('c4m_vocab_document_type:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('c4m_vocab_geo', 'c4m_vocab_geo');
    $this->addFieldMapping('c4m_vocab_geo:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('c4m_vocab_language', 'c4m_vocab_language');
    $this->addFieldMapping('c4m_vocab_language:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('c4m_vocab_topic', 'c4m_vocab_topic');
    $this->addFieldMapping('c4m_vocab_topic:create_term')
      ->defaultValue(TRUE);
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

    if ($this->hasCategories()) {
      $this->prepareRowCategoriesAndTags($row, 'categories');
    }

    if ($this->hasTags()) {
      $this->prepareRowCategoriesAndTags($row, 'tags');
    }

    return TRUE;
  }

  /**
   * Helper function to prepare the specific term fields (categories & tags).
   *
   * @param string $source_field
   *   The name of the source field.
   */
  protected function prepareRowCategoriesAndTags(&$row, $source_field) {
    if (!isset($row->{$source_field})) {
      return;
    }

    $row->{$source_field} = unserialize($row->{$source_field});

    // We need an array, even if it's empty.
    if (empty($row->{$source_field})) {
      $row->{$source_field} = array();
    }
    if (!is_array($row->{$source_field})) {
      $row->{$source_field} = array($row->{$source_field});
    }

    foreach ($row->{$source_field} as $index => $term_id) {
      // Remove this entry from the initial field.
      // Will be either moved to another field or replaced by a term name.
      unset($row->{$source_field}[$index]);

      $query = $this->createSelect('term_data', 'td');
      $query->addField('td', 'name');
      $query->condition('td.tid', $term_id);
      $results = $query->execute();

      while ($record = $results->fetchField()) {
        $termName = $record;

        // Remove term from original field and map to specific field instead.
        if ($mapping = $this->mappingCategoriesAndTagsContainsTermName($termName)) {
          $vocabulary_field = $mapping['vocabulary'];
          $row->{$vocabulary_field} = $mapping['terms'];
        }
        // Else, store the term name in the array instead of the original tid.
        else {
          $row->{$source_field}[$index] = $termName;
        }
      }
    }
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
    // Categories.
    if ($this->hasCategories()) {
      $this->prepareCategoriesAndTags($node, $row, 'categories');
    }

    // Tags.
    if ($this->hasTags()) {
      $this->prepareCategoriesAndTags($node, $row, 'tags');
    }
  }

  /**
   * Helper function to review a "categories" or "tags" data row after fetch.
   *
   * @param object $node
   *   The drupal node entity object.
   * @param object $row
   *   The source row object.
   * @param string $source_field
   *   The name of the source field.
   */
  protected function prepareCategoriesAndTags(&$node, &$row, $source_field) {
    $node->og_vocabulary[LANGUAGE_NONE] = array();

    if (!empty($row->{$source_field})) {
      $vocabulary_type = ($source_field == 'tags') ? 'c4m_vocab_tag' : 'c4m_vocab_category';

      foreach ($row->{$source_field} as $term_name) {
        $term = NULL;
        $vocabulary_name = NULL;

        if ($group_id = $this->getGroupIdOfContent($node)) {
          $vocabulary_name = $this->getGroupVocabularyName($group_id, $vocabulary_type);
          $term = taxonomy_get_term_by_name($term_name, $vocabulary_name);
        }

        if (empty($term)) {
          if (!empty($vocabulary_name)) {
            $vocabulary = taxonomy_vocabulary_machine_name_load($vocabulary_name);
            $term = new stdClass();
            $term->name = $term_name;
            $term->vid = $vocabulary->vid;
            $term->vocabulary_machine_name = $vocabulary_name;
            taxonomy_term_save($term);
          }
        }
        else {
          $term = reset($term);
        }

        if ($term && isset($term->tid) && is_numeric($term->tid)) {
          $node->og_vocabulary[LANGUAGE_NONE][] = array('target_id' => $term->tid);
        }
      }
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
   * Retrieve the Categories & Tags mapping array.
   */
  public function getMappingCategoriesAndTags() {
    if (is_null($this->mappingCategoriesAndTags)) {
      $mapping = c4d_migrate_categories_tags_csv_converter_get_mapping_array();
      $this->mappingCategoriesAndTags = $mapping;
    }

    if (empty($this->mappingCategoriesAndTags)) {
      $this->mappingCategoriesAndTags = array();
    }

    return $this->mappingCategoriesAndTags;
  }

  /**
   * Retrieves the entry in the Categories & Tags mapping array
   * with the given term name.
   *
   * @param string $termName
   *   The term name to search for.
   *
   * @return bool|array
   *   Returns the value of the mapping item with the given term name or
   *   FALSE if none is found.
   */
  public function mappingCategoriesAndTagsContainsTermName($termName) {
    $termName = htmlspecialchars_decode($termName);
    $mapping = $this->getMappingCategoriesAndTags();

    if (isset($mapping[$termName]) && !empty($mapping[$termName])) {
      return $mapping[$termName];
    }

    return FALSE;
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
    // Make sure the messages have the proper timestamp.
    $this->messageInsertSave($entity, $row);

    // Make sure the update message also exists.
    $this->messageUpdateSave($entity, $row);
  }

  /**
   * Returns the group ID of the group the node is associated with.
   *
   * @param object $node
   *   The node object.
   *
   * @return bool|mixed
   *   Returns the Group ID or FALSE if not found.
   */
  protected function getGroupIdOfContent($node) {
    if (
      isset($node->og_group_ref[LANGUAGE_NONE][0]) &&
      !is_null($node->og_group_ref[LANGUAGE_NONE][0]) &&
      isset($node->og_group_ref[LANGUAGE_NONE][0]['target_id']) &&
      is_numeric($node->og_group_ref[LANGUAGE_NONE][0]['target_id'])
    ) {
      return $node->og_group_ref[LANGUAGE_NONE][0];
    }

    return FALSE;
  }

  /**
   * Returns the vocabulary name associated with the given group.
   *
   * @param object $group
   *   The group to filter for.
   * @param $vocabulary_type
   *   The vocabulary type to search for.
   *
   * @return string
   *   The vocabulary machine name.
   */
  protected function getGroupVocabularyName($group, $vocabulary_type) {
    if (is_numeric($group)) {
      $group = node_load($group);
    }

    $vocabulary = c4m_og_vocab_load_group_vocabulary($group, $vocabulary_type);
    return $vocabulary->machine_name;
  }

}
