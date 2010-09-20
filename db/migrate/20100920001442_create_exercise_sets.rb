class CreateExerciseSets < ActiveRecord::Migration
  def self.up
    create_table :exercise_sets do |t|
      t.string :name
      t.integer :position
      t.integer :exercise_id

      t.timestamps
    end
  end

  def self.down
    drop_table :exercise_sets
  end
end
