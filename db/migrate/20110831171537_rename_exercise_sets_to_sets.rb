class RenameExerciseSetsToSets < ActiveRecord::Migration
  def self.up
    rename_table :exercise_sets, :sets
  end

  def self.down
    rename_table :sets, :exercise_sets
  end
end
