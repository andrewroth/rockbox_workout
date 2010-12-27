class Exercise < ActiveRecord::Base
  include Csv

  belongs_to :exercise_type
  has_many :exercise_sets

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:exercises).flatten.uniq
    end 
  end 

  def name_with_type
    "#{name} (#{exercise_type.try(:name)})"
  end
end
