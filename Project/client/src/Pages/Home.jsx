import { useMemo, useState } from 'react'
import { Navigate } from 'react-router-dom';
import {
  Box,
  Button,
  Card,
  CardActionArea,
  CardContent,
  Chip,
  Container,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormControl,
  Grid,
  InputLabel,
  MenuItem,
  Select,
  Stack,
  TextField,
  Typography,
} from '@mui/material'
import { useGlobal } from '../SessionManager';

const SAMPLE_TASKS = [
  'Draft sprint goals',
  'Update Kanban swimlanes',
  'Prep status meeting',
  'Review UX feedback',
  'Sketch new feature wireframes',
  'Finalize testing checklist',
  'Ship release notes',
  'Refine onboarding flow',
  'Clean up legacy tags',
  'Sync with marketing team',
]


const getRandomDueDateLabel = () => {
  const daysFromNow = Math.floor(Math.random() * 10) + 1
  const dueDate = new Date()
  dueDate.setDate(dueDate.getDate() + daysFromNow)
  return dueDate.toLocaleDateString(undefined, {
    month: 'short',
    day: 'numeric',
  })
}

const getRandomXP = () => {
  const xpBuckets = [40, 60, 80, 120, 150]
  return xpBuckets[Math.floor(Math.random() * xpBuckets.length)]
}

const buildTasks = (count = 3) =>
  Array.from({ length: count }).map((_, index) => ({
    id: `${Date.now()}-${index}-${Math.random().toString(36).slice(2, 6)}`,
    name: SAMPLE_TASKS[Math.floor(Math.random() * SAMPLE_TASKS.length)],
    dueDate: getRandomDueDateLabel(),
    xp: getRandomXP(),
  }))

const createBoard = (name, type) => ({
  id: `${type}-${name}-${Math.random().toString(36).slice(2, 8)}`,
  name,
  tasks: buildTasks(3),
  type,
})

function TaskboardDialog({ open, onClose, onSave }) {
  const [boardName, setBoardName] = useState('')
  const [boardType, setBoardType] = useState('personal')

  const handleClose = () => {
    setBoardName('')
    setBoardType('personal')
    onClose()
  }

  const handleSave = () => {
    if (!boardName.trim()) return
    onSave({ name: boardName.trim(), type: boardType })
    handleClose()
  }

  return (
    <Dialog open={open} onClose={handleClose} fullWidth maxWidth="sm">
      <DialogTitle>New Task Board</DialogTitle>
      <DialogContent>
        <Stack spacing={3} sx={{ pt: 1 }}>
          <TextField
            label="Board Name"
            value={boardName}
            onChange={(event) => setBoardName(event.target.value)}
            fullWidth
          />
          <FormControl fullWidth>
            <InputLabel id="board-type-label">Board Type</InputLabel>
            <Select
              labelId="board-type-label"
              label="Board Type"
              value={boardType}
              onChange={(event) => setBoardType(event.target.value)}
            >
              <MenuItem value="personal">Personal</MenuItem>
              <MenuItem value="shared">Shared</MenuItem>
            </Select>
          </FormControl>
        </Stack>
      </DialogContent>
      <DialogActions sx={{ px: 3, pb: 2 }}>
        <Button onClick={handleClose}>Cancel</Button>
        <Button variant="contained" onClick={handleSave}>
          Save Board
        </Button>
      </DialogActions>
    </Dialog>
  )
}

function BoardSection({ title, boards }) {
  return (
    <Box>
      <Typography variant="h5" fontWeight={600} sx={{ mb: 2 }}>
        {title}
      </Typography>
      <Grid container spacing={3}>
        {boards.map((board) => (
          <Grid item xs={12} md={6} key={board.id}>
            <Card
              elevation={3}
              sx={{
                borderRadius: 3,
                height: '100%',
                background:
                  'linear-gradient(135deg, rgba(25, 118, 210, 0.08), rgba(255, 112, 67, 0.08))',
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
                      board.type === 'personal'
                        ? 'Personal Board'
                        : 'Shared Board'
                    }
                    color={board.type === 'personal' ? 'primary' : 'secondary'}
                    size="small"
                    sx={{ alignSelf: 'flex-start' }}
                  />
                </Box>
                <Stack spacing={2}>
                  {board.tasks.map((task) => (
                    <CardActionArea key={task.id} disableRipple>
                      <Card
                        variant="outlined"
                        sx={{
                          borderRadius: 2,
                          p: 2,
                          transition: 'transform 0.2s ease, box-shadow 0.2s ease',
                          '&:hover': {
                            transform: 'translateY(-2px)',
                            boxShadow: 6,
                            borderColor: 'primary.light',
                          },
                        }}
                      >
                        <Typography variant="subtitle1" fontWeight={600}>
                          {task.name}
                        </Typography>
                        <Typography
                          variant="body2"
                          color="text.secondary"
                          sx={{ mt: 0.5 }}
                        >
                          Due {task.dueDate}
                        </Typography>
                      </Card>
                    </CardActionArea>
                  ))}
                </Stack>
              </CardContent>
            </Card>
          </Grid>
        ))}
      </Grid>
    </Box>
  )
}

function Home() {
  const { state } = useGlobal()
  if (!state.loggedIn) {
    return (
        <Navigate to="/login" replace />
      )
  }

  const [isDialogOpen, setIsDialogOpen] = useState(false)
  const initialPersonalBoards = useMemo(
    () => [
      createBoard('Personal Board 1', 'personal'),
      createBoard('Personal Board 2', 'personal'),
      createBoard('Personal Board 3', 'personal'),
    ],
    []
  )
  const initialSharedBoards = useMemo(
    () => [
      createBoard('Shared Board A', 'shared'),
      createBoard('Shared Board B', 'shared'),
      createBoard('Shared Board C', 'shared'),
    ],
    []
  )
  const [personalBoards, setPersonalBoards] = useState(initialPersonalBoards)
  const [sharedBoards, setSharedBoards] = useState(initialSharedBoards)

  const openDialog = () => setIsDialogOpen(true)
  const closeDialog = () => setIsDialogOpen(false)

  const handleSaveBoard = ({ name, type }) => {
    const board = createBoard(name, type)
    if (type === 'personal') {
      setPersonalBoards((prev) => [...prev, board])
    } else {
      setSharedBoards((prev) => [...prev, board])
    }
  }

  return (
    <Container maxWidth="lg" sx={{ py: 6 }}>
      <Stack spacing={6}>
        <Box
          sx={{
            display: 'flex',
            flexDirection: { xs: 'column', md: 'row' },
            justifyContent: 'space-between',
            alignItems: { xs: 'flex-start', md: 'center' },
            gap: 2,
          }}
        >
          <Box>
            <Typography variant="h3" fontWeight={700}>
              Wat to Do Dashboard
            </Typography>
            <Typography variant="body1" color="text.secondary" sx={{ mt: 1 }}>
              Organize your personal focus and collaborate on shared priorities.
            </Typography>
          </Box>
          <Button
            variant="contained"
            size="large"
            onClick={openDialog}
            sx={{ alignSelf: { xs: 'stretch', md: 'center' } }}
          >
            + New Board
          </Button>
        </Box>

        <BoardSection title="Personal Task Boards" boards={personalBoards} />
        <BoardSection title="Shared Task Boards" boards={sharedBoards} />
      </Stack>

      <TaskboardDialog
        open={isDialogOpen}
        onClose={closeDialog}
        onSave={handleSaveBoard}
      />
    </Container>
  )
}

export default Home