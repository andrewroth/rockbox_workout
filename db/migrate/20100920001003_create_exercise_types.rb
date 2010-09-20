class CreateExerciseTypes < ActiveRecord::Migration
  def self.up
    create_table :exercise_types do |t|
      t.string :name
      t.string :v1
      t.string :v2
      t.string :v3
      t.string :v4
      t.string :v5
      t.string :v6
      t.string :v7

      t.timestamps
    end
  end

  def self.down
    drop_table :exercise_types
  end
end
