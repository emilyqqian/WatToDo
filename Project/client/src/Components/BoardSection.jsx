import {
  Box,
  Card,
  CardActionArea,
  CardContent,
  Chip,
  Grid,
  Stack,
  Typography,
  LinearProgress,
} from '@mui/material'

export default function BoardSection({ title, boards, onBoardClick }) {
  // Calculate completion percentage for each board
  const getCompletionPercentage = (board) => {
    if (!board.tasks || board.tasks.length === 0) return 0
    const completed = board.tasks.filter((t) => t.finished).length
    return Math.round((completed / board.tasks.length) * 100)
  }

  // Get gradient based on board type
  const getGradient = (isShared) => {
    if (isShared) {
      return 'linear-gradient(135deg, rgba(16, 185, 129, 0.08), rgba(59, 130, 246, 0.08))'
    }
    return 'linear-gradient(135deg, rgba(102, 126, 234, 0.08), rgba(118, 75, 162, 0.08))'
  }

  return (
    <Box>
      <Typography
        variant="h5"
        fontWeight={700}
        sx={{
          mb: 2.5,
          background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
          backgroundClip: 'text',
          WebkitBackgroundClip: 'text',
          WebkitTextFillColor: 'transparent',
        }}
      >
        {title}
      </Typography>

      {boards && boards.length > 0 ? (
        <Grid container spacing={2.5}>
          {boards.map((board) => {
            const completionPercent = getCompletionPercentage(board)
            const isShared = board.isShared

            return (
              <Grid item xs={12} key={board.taskboard_id}>
                <CardActionArea
                  onClick={() => onBoardClick && onBoardClick(board)}
                  sx={{ borderRadius: '0px' }}
                >
                  <Card
                    elevation={0}
                    sx={{
                      background: '#1a1f3a',
                      backdropFilter: 'none',
                      border: '3px solid rgba(255, 255, 255, 0.2)',
                      borderRadius: '0px',
                      transition: 'all 0.3s cubic-bezier(0.4, 0, 0.2, 1)',
                      boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.15)',
                      '&:hover': {
                        transform: 'translate(2px, 2px)',
                        boxShadow: '2px 2px 0px rgba(0, 0, 0, 0.50)',
                        borderColor: 'rgba(255, 255, 255, 0.3)',
                      },
                    }}
                  >
                    <CardContent sx={{ p: 3 }}>
                      <Stack spacing={2}>
                        {/* Header */}
                        <Box
                          sx={{
                            display: 'flex',
                            justifyContent: 'space-between',
                            alignItems: 'start',
                          }}
                        >
                          <Box flex={1}>
                            <Typography
                              variant="subtitle1"
                              fontWeight={700}
                              sx={{ mb: 0.5 }}
                            >
                              {board.name}
                            </Typography>
                            <Stack direction="row" spacing={1} sx={{ flexWrap: 'wrap' }}>
                              <Chip
                                label={isShared ? 'Shared' : 'Personal'}
                                size="small"
                                variant="filled"
                                sx={{
                                  background: isShared
                                    ? 'rgba(16, 185, 129, 0.2)'
                                    : 'rgba(102, 126, 234, 0.2)',
                                  color: isShared ? '#059669' : '#667eea',
                                  fontWeight: 600,
                                }}
                              />
                              <Chip
                                label={`${board.tasks?.length || 0} tasks`}
                                size="small"
                                variant="outlined"
                                sx={{ borderColor: 'rgba(255, 255, 255, 0.3)' }}
                              />
                            </Stack>
                          </Box>
                        </Box>

                        {/* Progress */}
                        <Box>
                          <Stack
                            direction="row"
                            justifyContent="space-between"
                            sx={{ mb: 1 }}
                          >
                            <Typography variant="body2" fontWeight={600}>
                              Progress
                            </Typography>
                            <Typography variant="body2" color="text.secondary">
                              {completionPercent}%
                            </Typography>
                          </Stack>
                          <LinearProgress
                            variant="determinate"
                            value={completionPercent}
                            sx={{
                              height: 8,
                              borderRadius: '0px',
                              border: '1px solid rgba(255, 255, 255, 0.3)',
                              background: 'rgba(255, 255, 255, 0.2)',
                              '& .MuiLinearProgress-bar': {
                                background: isShared
                                  ? 'linear-gradient(90deg, #10b981 0%, #3b82f6 100%)'
                                  : 'linear-gradient(90deg, #667eea 0%, #764ba2 100%)',
                                borderRadius: '0px',
                              },
                            }}
                          />
                        </Box>

                        {/* Tasks Preview */}
                        {board.tasks && board.tasks.length > 0 && (
                          <Stack spacing={1}>
                            <Typography variant="caption" fontWeight={600}>
                              Recent Tasks:
                            </Typography>
                            {board.tasks.slice(0, 2).map((task, idx) => (
                              <Box
                                key={idx}
                                sx={{
                                  p: 1.5,
                                  background: '#0f1225',
                                  borderRadius: '8px',
                                  display: 'flex',
                                  alignItems: 'center',
                                  gap: 1,
                                }}
                              >
                                <Box
                                  sx={{
                                    width: 4,
                                    height: 4,
                                    borderRadius: '50%',
                                    background: task.finished
                                      ? '#10b981'
                                      : '#f59e0b',
                                  }}
                                />
                                <Typography
                                  variant="body2"
                                  sx={{
                                    textDecoration: task.finished
                                      ? 'line-through'
                                      : 'none',
                                    opacity: task.finished ? 0.6 : 1,
                                    overflow: 'hidden',
                                    textOverflow: 'ellipsis',
                                    whiteSpace: 'nowrap',
                                  }}
                                >
                                  {task.title}
                                </Typography>
                              </Box>
                            ))}
                          </Stack>
                        )}
                      </Stack>
                    </CardContent>
                  </Card>
                </CardActionArea>
              </Grid>
            )
          })}
        </Grid>
      ) : (
        <Card
          elevation={0}
          sx={{
            background: 'linear-gradient(135deg, rgba(102, 126, 234, 0.08), rgba(118, 75, 162, 0.08))',
            backdropFilter: 'blur(10px)',
            border: '2px dashed rgba(255, 255, 255, 0.3)',
            borderRadius: '16px',
            p: 4,
            textAlign: 'center',
          }}
        >
          <Typography color="text.secondary" variant="body2">
            No boards yet. Create your first board to get started! ✨
          </Typography>
        </Card>
      )}
    </Box>
  )
}