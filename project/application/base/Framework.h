#pragma once

/// <summary>
/// ゲーム全体の骨組み
/// </summary>
class Framework
{
public:
	virtual ~Framework() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	virtual bool IsEndRequest() { return endRequest_; }

public:

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

protected:

	bool endRequest_ = false;

};

