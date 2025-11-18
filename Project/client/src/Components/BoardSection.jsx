import {
  Box,
  Card,
  CardActionArea,
  CardContent,
  Chip,
  Grid,
  Stack,
  Typography,
} from '@mui/material'

export default function BoardSection({ title, boards, onBoardClick }) {
  return (
    <Box>
      <Typography variant="h5" fontWeight={600} sx={{ mb: 2 }}>
        {title}
      </Typography>
      <Grid container spacing={3}>
        {boards.map((board) => (
          <Grid item xs={12} md={6} key={board.taskboard_id}>
            <CardActionArea
              onClick={() => onBoardClick && onBoardClick(board)}
              sx={{ height: '100%' }}
            >
              <Card
                elevation={3}
                sx={{
                  borderRadius: 3,
                  height: '100%',
                  background:
                    'linear-gradient(135deg, rgba(25, 118, 210, 0.08), rgba(255, 112, 67, 0.08))',
                    transition:
                            'transform 0.2s ease, box-shadow 0.2s ease',
                          '&:hover': {
                            transform: 'translateY(-2px)',
                            boxShadow: 6,
                            borderColor: 'primary.light',
                          },
                }}
              >
                <CardContent sx={{ p: 3 }}>
                  <Box
                    sx={{
                      display: 'flex',
                      flexDirection: 'column',
                      gap: 1,
                      mb: 2,
                    }}
                  >
                    <Typography variant="subtitle1" fontWeight={600}>
                      {board.name}
                    </Typography>
                    <Chip
                      label={
                        board.isShared
                          ? 'Shared Board'
                          : 'Personal Board'
                      }
                      color={board.isShared ? 'primary' : 'secondary'}
                      size="small"
                      sx={{ alignSelf: 'flex-start' }}
                    />
                  </Box>
                  <Stack spacing={2}>
                    {board.tasks.map((task) => (
                      <Card
                        key={task.task_id}
                        variant="outlined"
                        sx={{
                          borderRadius: 2,
                          p: 2,
                          transition:
                            'transform 0.2s ease, box-shadow 0.2s ease',
                          '&:hover': {
                            transform: 'translateY(-2px)',
                            boxShadow: 6,
                            borderColor: 'primary.light',
                          },
                        }}
                      >
                        <Typography variant="subtitle1" fontWeight={600}>
                          {task.title}
                        </Typography>
                        <Typography
                          variant="body2"
                          color="text.secondary"
                          sx={{ mt: 0.5 }}
                        >
                          Due {task.due_date}
                        </Typography>
                      </Card>
                    ))}
                  </Stack>
                </CardContent>
              </Card>
            </CardActionArea>
          </Grid>
        ))}
      </Grid>
    </Box>
  )
}